#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"
#define M_PI           3.14159265358979323846

namespace WAV {
#pragma pack(push, 1)
	struct WavHeader
	{
		char chunk_id[4];
		//char chunk_id[4] = { 'R', 'I', 'F', 'F' };
		uint32_t chunk_size;
		char format[4];
		//char format[4] = { 'W', 'A', 'V','E' };

		//fmt subchunk
		char subchunk1_id[4];
		//char subchunk1_id[4] = { 'f', 'm', 't',' ' };
		uint32_t subchunk1_size;
		//uint32_t subchunk1_size = 16;
		//uint16_t subchunk1_format = 1; //PCM
		uint16_t subchunk1_format; //PCM
		uint16_t num_channels;
		//uint16_t num_channels = 2;

		uint32_t sample_rate;
		//uint32_t sample_rate = 44100;

		//uint32_t byte_rate = sample_rate * num_channels * (subchunk1_size / 8);
		uint32_t byte_rate;

		uint16_t block_align;
		//uint16_t block_align = num_channels * (subchunk1_size / 8);

		uint16_t bits_per_sample;
		//uint16_t bits_per_sample = 16;

		//data subchunk
		//char subchunk2_id[4] = { 'd','a','t','a' };
		char subchunk2_id[4];
		uint32_t subchunk2_size;


		unsigned int calcSize() {
		}
	};
#pragma pack(pop)


#pragma pack(push, 1)

	struct CompressedWavHeader {
		char codec_tag[4] = { 'M', 'U', 'C', '1' };

		char window_name[4] = { 'H', 'A', 'N', 'N' };
		int version = 1;

		int sample_rate;
		int channels;
		int bits_per_sample;

		int window_size;   // 2N
		//int mdct_size;     // windowSize / 2 

		int quant_levels;  // Q 

		int total_frames;
		int total_samples;
	};
#pragma pack(pop)


#pragma pack(push, 1)
	struct Band {
		int start;
		int end;
	};
#pragma pack(pop)

	class WAVFile {
	private:

		std::vector<Band> bands = {
			{0, 4},
			{4, 8},
			{8, 16},
			{16, 32},
			{32, 64},
			{64, 128},
			{128, 256},
			{256, 512},
			{512, 1024}
		};


		const int windowSize = 2048;

		WAV::WavHeader header;
		int Q = 50; // степень квантования( качество сжатия)
		std::vector<char> data;
		std::vector<float> samplesLeft;
		std::vector<float> samplesRight;
		std::vector<float> window;

		std::vector<float> hannWindow;
		std::vector<ConvertedSample> convertedChunks;
		std::string fileName;
		std::vector<std::vector<float>> cosTable;
		const char extension[4] = { 'M', 'U','X','3' };
		int currPosition = 0;
		bool headerSet = false;

	public:

		WAVFile() {
			window.resize(windowSize);
			hannWindow.resize(windowSize);
		}
		int getQ() {
			return this->Q;
		}
		int setQ(int newQ) {
			this->Q = newQ;
			return this->Q;
		}
		bool read(const char* path) {
			// open file
			std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

			if (!file.is_open()) {
				return false;
			}
			// get the file size
			std::ifstream::pos_type fileSize = file.tellg();
			if (fileSize < 0) {
				return false;
			}
			// move cursos to beg. 
			file.seekg(0, std::ios::beg);

			// read data to buffer
			data = std::vector<char>(fileSize); // init vector with empty cells 
			file.read(&data[0], fileSize);

			// copy first 44 bytes for the WAV header
			std::memcpy(&header, &data[0], sizeof(WAV::WavHeader));
			this->headerSet = true;
			for (int i = 0; i < sizeof(WAV::WavHeader); i++) {
				data.erase(data.begin());
			}


			data.shrink_to_fit();
			std::cout << "Map Started\n";
			this->initCosTable(windowSize / 2);
			std::cout << "\nMap Ended \n";
			std::cout << "Raw Data started \n";
			this->processRawData();
			std::cout << "Raw Data ended\n";
			file.close();
			return true;
		}

		bool write(const char* path) {
			this->recalcHeaderSize();

			std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::ate);
			if (!file.is_open()) {
				return false;
			}
			file.write(reinterpret_cast<char *>(&header), sizeof(WAV::CompressedWavHeader));

			file.write(&data[0], data.size());
		}


		WAV::WAVFile & setSaveFileName(std::string name) {
			this->fileName = name;
			return *this;
		}
		std::vector<char> getData() {
			return this->data;
		}

		WAV::WavHeader & getHeader() {
			return this->header;
		}
		void viewHeaders() {
			std::cout << header.bits_per_sample << "\n";
			std::cout << header.block_align << "\n";
			std::cout << header.byte_rate << "\n";
			std::cout << header.chunk_id << "\n";
			std::cout << header.chunk_size << "\n";
			std::cout << header.format << "\n";
			std::cout << header.num_channels << "\n";
			std::cout << header.sample_rate << "\n";
			std::cout << header.subchunk1_format << "\n";
			std::cout << header.subchunk1_id << "\n";
			std::cout << header.subchunk1_size << "\n";
			std::cout << header.subchunk2_id << "\n";
			std::cout << header.subchunk2_size << "\n";
		}
		WAVFile* recalcHeaderSize() {

			return this;
		}
		void processRawData() { // возможно нужен фикс!!!!!!!
			
			if (headerSet == false) {
				return;
			}
			int max = 0;
			int min = 0;
			int bytesPerSample = this->header.bits_per_sample / 8;
			int frameSize = header.num_channels * bytesPerSample;
			int maxVal = (1 << header.bits_per_sample - 1);
			for (int i = 0; i + frameSize < data.size(); i = i + frameSize) {
				for (int ch = 0; ch < header.num_channels; ch++) {
					int offset = i + ch * bytesPerSample; // здесь: i + ch + bytesPerSample 
					int32_t sample = 0;
					for (int b = 0; b < bytesPerSample; b++) {
						sample |= static_cast<uint8_t>(data[offset + b]) << (8 * b);
					}
					int shift = 32 - header.bits_per_sample;
					sample = (sample << shift) >> shift;
					if (sample > max) max = sample;
					if (sample < min) min = sample;
					if (ch == 0) {
						samplesLeft.push_back((float)sample / maxVal);
					}
					else if (ch == 1) {
						samplesRight.push_back((float)sample / maxVal);
					}

				}
			}

			std::cout << "min: " << min << "\n";
			std::cout << "max: " << max << "\n";
			this->convertRawData();
		}
		void sampleTest() {
			if (this->samplesLeft.size() <= 0) {
				return;
			}
			for (int i = 0; i < 30; i++) {

				std::cout << "Left: " << this->samplesLeft[i] << "\n";
				std::cout << "Right: " << this->samplesRight[i] << "\n";
			}
		}




		bool convertRawData() {

			if (samplesLeft.empty() || samplesRight.empty())
				return false;

			int hopSize = windowSize / 2; // overlap
			std::vector<float> spectrum;
			std::vector<int> quantized;
			std::vector<float> scaleFactors;
			int loopCounter = 0;
			for (int pos = 0; pos < samplesLeft.size(); pos += hopSize) {
				/*
				if (loopCounter >= 10 && true == true) {
					std::cout << "break";
					break;
				}
				*/
				spectrum.clear();
				quantized.clear();
				scaleFactors.clear();

				this->encodeFrame(spectrum, quantized, scaleFactors);
				loopCounter++;

			}
			std::cout << "Done";
			return true;
		}


		void encodeFrame(std::vector<float> &spectrum, std::vector<int> &quantized, std::vector<float> scaleFactors) {
			spectrum.clear();
			quantized.clear();
			scaleFactors.clear();
			this->handleHannWindow();


			this->MDCT(window, spectrum);


			this->quantization(spectrum, quantized, scaleFactors);
			this->convertedChunks.push_back(WAV::ConvertedSample().setConvertedChunk(quantized).setScaleFactors(scaleFactors));
		}

		bool writeDataToFile() {
			CompressedWavHeader newWAVheader = this->prepareCustomHeader();
			return false;
		}

		void handleHannWindow() {
			this->fillWindow();
			this->fillHannWindowValues();
			this->applyHannWindow();
		}
		void fillWindow(int currPos = 0) {

			for (int i = 0; i < windowSize; i++) {

				int idx = currPos + i;

				if (idx >= samplesLeft.size()) {
					window[i] = 0.0f;
					continue;
				}

				float l = samplesLeft[idx];
				float r = samplesRight[idx];

				window[i] = 0.5f * (l + r);
			}
			std::cout << "Window\n";
			for (int i = 0; i < windowSize; i++) {
				std::cout << window[i] << "\n";
			}

		}
		void applyHannWindow() {

			for (int i = 0; i < windowSize; i++) {
				window[i] *= hannWindow[i];
			}
		}
		// hann window creation
		void fillHannWindowValues() {
			hannWindow.clear();
			float factor = 2.0f * M_PI / (windowSize - 1);
			for (int i = 0; i < windowSize; i++) {
				// float w = 0.5f * (1.0f - cos(2.0f * M_PI * i / (windowSize - 1)));
				float w = 0.5f * (1.0f - cos(factor * i));
				hannWindow.push_back(w);
			}
		}
		// decide if we need to fill the window
		void makeHannWindow() { // depricated
			if (this->hannWindow.size() != windowSize) {
				fillHannWindowValues();
			}

		}

		void mdct_test() {

			int N = 1024;

			std::vector<float> input(2 * N);
			std::vector<float> spectrum;

			mdct_test_sine_gen(input, 440.0f, 44100);

			MDCT(input, spectrum);

			for (int i = 0; i < 1024; i++) {
				printf("%f\n", fabs(spectrum[i]));
			}
		}



		void mdct_test_sine_gen(std::vector<float>& input, float freq, int sample_rate) {
			for (int i = 0; i < input.size(); i++)
			{
				input[i] = sin(2 * M_PI * freq * i / sample_rate);
			}
		}

		// transform (time, value) to (frenquency)
		// result is an array of input.size()/2
		// i can use the output to show graph in openGL
		void MDCT(std::vector<float>& input, std::vector<float>& output) {
			int N = input.size() / 2;
			std::cout << "MDCT\n";
			for (int i = 0; i < input.size(); i++) {
				std::cout << input[i] << "\n";
			}
			//system("pause");
			output.resize(N);

			for (int k = 0; k < N; k++) {
				float sum = 0.0f;
				for (int n = 0; n < 2 * N; n++) {
					sum += input[n] * cosTable[k][n];
				}
				output[k] = sum;
			}

		}



		// quantization
		// 1 - split output from MDCT into bands and quant them separetely
		// 2 - normalise data and apply quantization
		// 3 - convert float data to int
		void quantization(std::vector<float> &input,
			std::vector<int> &output,
			std::vector<float> &scaleFactors)
		{
			if (input.empty())
				return;

			output.resize(input.size());
			scaleFactors.clear();

			//const float Q = 127.0f; // например, 8-бит квантование

			for (int b = 0; b < bands.size(); b++) {

				int start = bands[b].start;
				int end = bands[b].end;
				if ((int)input.size() < bands[b].end) {
					end = (int)input.size();
				}


				float maxVal = 0.0f;

				// 1. найти максимум
				for (int i = start; i < end; i++) {
					float val = fabs(input[i]);
					if (val > maxVal)
						maxVal = val;
				}


				if (maxVal == 0.0f)
					maxVal = 1.0f;

				scaleFactors.push_back(maxVal);

				// 2. квантование
				for (int i = start; i < end; i++) {
					output[i] = (int)(input[i] / maxVal * Q);
				}
			}
		}
		void initCosTable(int N) {

			cosTable.resize(N, std::vector<float>(2 * N));

			for (int k = 0; k < N; k++) {
				for (int n = 0; n < 2 * N; n++) {

					float angle = M_PI / N *
						(n + 0.5f + N / 2.0f) *
						(k + 0.5f);

					cosTable[k][n] = cosf(angle);
				}
			}
		}


	};
}

