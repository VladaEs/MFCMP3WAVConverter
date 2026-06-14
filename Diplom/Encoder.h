#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <functional>
#include "ConvertedSample.h"
#include "MUCFile.h"
#include "WAVFile.h"
#include "CodecContext.h"

#define M_PI           3.14159265358979323846




class Encoder
{
private:

	CodecContext& config = CodecContext::Instance();

	std::vector<Band> bands = config.getBands();

	std::function<void(int)> progressCallback;
	const int windowSize = config.getWindowSize();

	int Q = config.getQ(); // степень квантования( качество сжатия)
	
	std::vector<float> samplesLeft;
	std::vector<float> samplesRight;
	std::vector<float> window;

	std::vector<float> hannWindow;
	std::vector<WAV::ConvertedSample> convertedChunks;
	std::vector<std::vector<float>> cosTable = config.getCosTable();
	const char extension[3] = { 'M', 'U','C' };
	int currPosition = 0;
	//bool headerSet = false;



public:
	Encoder() {
		window.resize(windowSize);
	}
	Encoder(const std::vector<char>& data, int bitsPerSample, int numChannels) : Encoder() {
		Encoder();
		this->processRawData(data, bitsPerSample, numChannels);
		
	}
	MUC::MUCFile Encode(const std::vector<char>& data, int bitsPerSample, int numChannels) {
		MUC::MUCFile file;
		this->processRawData(data, bitsPerSample, numChannels);
		file.setConvertedData(convertedChunks);
	}
	MUC::MUCFile Encode(WAV::WAVFile & wav) {
		MUC::MUCFile fileMUC;
		this->processRawData(wav.getData(), wav.getBitsPerSample(), wav.getNumChannels());
		fileMUC.setConvertedData(convertedChunks);
		fileMUC.setCustomHeader(this->prepareCustomHeader(wav));
		return fileMUC;
	}






	bool processRawData(const std::vector<char>& data, int bitsPerSample, int numChannels ) {
		/*
		if (headerSet == false) {
			return false;
		}
		*/
		int max = 0;
		int min = 0;
		int bytesPerSample = bitsPerSample / 8;
		int frameSize = numChannels * bytesPerSample;
		int maxVal = (1 << bitsPerSample - 1);
		for (int i = 0; i + frameSize < data.size(); i = i + frameSize) {
			for (int ch = 0; ch < numChannels; ch++) {
				int offset = i + ch * bytesPerSample; // здесь: i + ch + bytesPerSample
				int32_t sample = 0;
				for (int b = 0; b < bytesPerSample; b++) {
					sample |= static_cast<uint8_t>(data[offset + b]) << (8 * b);
				}
				int shift = 32 - numChannels;
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

		TRACE("MIN: %d; MAX: %d \n\n\n" , min, max);
		if (this->convertRawData()) {
			return true;
		}
		return false;
	}

	bool convertRawData() {

		if (samplesLeft.empty() || samplesRight.empty())
			return false;

		int hopSize = windowSize / 2; // overlap
		std::vector<float> spectrum;
		std::vector<int8_t> quantized;
		std::vector<float> scaleFactors;
		int loopCounter = 0;
		TRACE("left: %d Right: %d", samplesLeft.size(), samplesRight.size());
		for (int pos = 0; pos + windowSize < samplesLeft.size(); pos += hopSize) {
			/*
			if (loopCounter >= 10 && true == true) {
				std::cout << "break";
				break;
			}
			*/
			spectrum.clear();
			quantized.clear();
			scaleFactors.clear();
			int percent = (100 * pos) / samplesLeft.size();
			
			if (progressCallback) {
				progressCallback(percent);
			}

			this->encodeFrame(pos, spectrum, quantized, scaleFactors);
			loopCounter++;

		}
		TRACE("COnvert Raw Data Done");
		return true;
	}


	void encodeFrame(size_t currPos,  std::vector<float> &spectrum, std::vector<int8_t> &quantized, std::vector<float> scaleFactors) {
		this->handleHannWindow(currPos);


		this->MDCT(window, spectrum);


		this->quantization(spectrum, quantized, scaleFactors);
		this->convertedChunks.push_back(WAV::ConvertedSample().setConvertedChunk(quantized).setScaleFactors(scaleFactors));
	}
	void setProgressCallback( std::function<void(int)> cb){
		progressCallback = cb;
	}


	void handleHannWindow(size_t pos) {
		this->fillWindow(pos);
		this->fillHannWindowValues();
		this->applyHannWindow();
	}
	void fillWindow(int currPos = 0) {
		TRACE(
			"window size = %zu\n",
			window.size());
		for (int i = 0; i < windowSize; i++) {

			int idx = currPos + i;

			if (idx >= samplesLeft.size()) {
				window[i] = 0.0f;
				continue;
			}

			float l = samplesLeft[idx];
			float r = samplesRight[idx];

			window[i] = 0.5f * (l + r);
			TRACE("\nWindow value: %f\n", window[i]);
		}
		TRACE("Window\n");
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
	void quantization(std::vector<float> &input, std::vector<int8_t> &output, std::vector<float> &scaleFactors)
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
			if (maxVal == 0.0f) {
				maxVal = 1.0f;
			}
			scaleFactors.push_back(maxVal);
			// 2. квантование
			for (int i = start; i < end; i++) {
				output[i] = (int)(input[i] / maxVal * Q);
			}
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

	void sampleTest() {
		if (this->samplesLeft.size() <= 0) {
			return;
		}
		for (int i = 0; i < 30; i++) {

			std::cout << "Left: " << this->samplesLeft[i] << "\n";
			std::cout << "Right: " << this->samplesRight[i] << "\n";
		}
	}

	MUC::MUCHeader prepareCustomHeader(WAV::WAVFile &wav) {
		MUC::MUCHeader header;
		header.sample_rate = wav.getSampleRate();
		header.channels = wav.getNumChannels();
		header.bits_per_sample = wav.getBitsPerSample();
		header.window_size = this->windowSize;
		header.total_frames = convertedChunks.size();
		header.total_samples = samplesLeft.size();
		header.bands_amount = bands.size();
		return header;
	}

};

