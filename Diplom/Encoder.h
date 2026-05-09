#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"
#define M_PI           3.14159265358979323846


#pragma pack(push, 1)
struct Band {
	int start;
	int end;
};
#pragma pack(pop)


class Encoder
{
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

	int Q = 50; // степень квантования( качество сжатия)
	std::vector<char> data; // remove
	std::vector<float> samplesLeft;
	std::vector<float> samplesRight;
	std::vector<float> window;

	std::vector<float> hannWindow;
	std::vector<WAV::ConvertedSample> convertedChunks;
	std::string fileName; // remove
	std::vector<std::vector<float>> cosTable;
	const char extension[4] = { 'M', 'U','X','3' };
	int currPosition = 0;
	bool headerSet = false;

	
	bool processRawData(std::vector<char> data, int bitsPerSample, int numChannels ) { 
		if (headerSet == false) {
			return;
		}
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

		std::cout << "min: " << min << "\n";
		std::cout << "max: " << max << "\n";
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

