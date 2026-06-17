#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <functional>
#include <algorithm>
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
	std::vector<float> cosTable = config.getCosTable();
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
		TRACE("Original data size = %zu\n", wav.getData().size());
		TRACE("Sample rate = %d\n", wav.getSampleRate());
		TRACE("Channels = %d\n", wav.getNumChannels());
		TRACE("Bits = %d\n", wav.getBitsPerSample());

		this->processRawData(wav.getData(), wav.getBitsPerSample(), wav.getNumChannels());
		TestMDCT();
		TRACE("Left samples  = %zu\n", samplesLeft.size());
		TRACE("Right samples = %zu\n", samplesRight.size());
		fileMUC.setConvertedData(convertedChunks);
		TRACE("Frames encoded = %zu\n", convertedChunks.size());
		fileMUC.setCustomHeader(this->prepareCustomHeader(wav));
		return fileMUC;
	}


	std::vector<float> IMDCT(std::vector<float>& input)
	{
		int N = static_cast<int>(input.size());

		std::vector<float> res(windowSize);

		const float* in = input.data();
		const float* table = cosTable.data();

		for (int n = 0; n < windowSize; n++)
		{
			float sum = 0.0f;

			for (int k = 0; k < N; k++)
			{
				const float* row =
					table + k * windowSize;

				sum += in[k] * row[n];
			}

			res[n] = sum;
		}

		return res;
	}
	void TestMDCT()
	{
		fillWindow(0);

		std::vector<float> spectrum;
		MDCT(window, spectrum);

		std::vector<float> restored =
			IMDCT(spectrum);

		TRACE("\n===== MDCT TEST =====\n");

		float inMin = FLT_MAX;
		float inMax = -FLT_MAX;

		for (float v : window)
		{
			if (v < inMin) inMin = v;
			if (v > inMax) inMax = v;
		}

		float outMin = FLT_MAX;
		float outMax = -FLT_MAX;

		for (float v : restored)
		{
			if (v < outMin) outMin = v;
			if (v > outMax) outMax = v;
		}

		TRACE(
			"INPUT RANGE  = %f .. %f\n",
			inMin,
			inMax);

		TRACE(
			"OUTPUT RANGE = %f .. %f\n",
			outMin,
			outMax);

		TRACE("\n===== FIRST 20 SAMPLES =====\n");

		for (int i = 0; i < 20; i++)
		{
			TRACE(
				"%d: in=%f out=%f\n",
				i,
				window[i],
				restored[i]);
		}

		double mse = 0.0;

		size_t count = window.size();

		if (restored.size() < count)
		{
			count = restored.size();
		}

		for (size_t i = 0; i < count; i++)
		{
			double diff = window[i] - restored[i];
			mse += diff * diff;
		}
		mse /= window.size();

		TRACE(
			"\nMSE = %f\n",
			mse);

		TRACE(
			"=====================\n");
	}


	bool processRawData(const std::vector<char>& data, int bitsPerSample, int numChannels ) {
		/*
		if (headerSet == false) {
			return false;
		}
		*/
		if (numChannels <= 1) {
			return true;
		}
		int debugCounter = 0;
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
				int shift = 32 - bitsPerSample;
				sample = (sample << shift) >> shift;

				int rawSample = sample;
				if (debugCounter < 20)
				{
					TRACE(
						"raw=%d signed=%d\n",
						rawSample,
						sample);

					debugCounter++;
				}
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
		TRACE("MIN: %d; MAX: %d \n", min, max);

		TRACE("\n===== START =====\n");
		for (int i = 0; i < 10; i++)
		{
			TRACE("%f\n", samplesLeft[i]);
		}

		TRACE("\n===== MIDDLE =====\n");
		int mid = (int)samplesLeft.size() / 2;
		for (int i = 0; i < 10; i++)
		{
			TRACE("%f\n", samplesLeft[mid + i]);
		}

		TRACE("\n===== END =====\n");
		int end = (int)samplesLeft.size() - 10;
		for (int i = 0; i < 10; i++)
		{
			TRACE("%f\n", samplesLeft[end + i]);
		}
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


	void encodeFrame(
		size_t currPos,
		std::vector<float>& spectrum,
		std::vector<int8_t>& quantized,
		std::vector<float>& scaleFactors)
	{
		auto t0 = std::chrono::steady_clock::now();

		this->handleHannWindow(currPos);

		auto t1 = std::chrono::steady_clock::now();

		this->MDCT(window, spectrum);

		float mn = FLT_MAX;
		float mx = -FLT_MAX;

		for (float v : spectrum)
		{
			if (v < mn) mn = v;
			if (v > mx) mx = v;
		}

		TRACE(
			"MDCT range: %f .. %f\n",
			mn,
			mx);


		auto t2 = std::chrono::steady_clock::now();
		
		this->quantization( spectrum, quantized, scaleFactors);

		int qMin = 127;
		int qMax = -127;

		for (int8_t v : quantized)
		{
			if ((int)v < qMin)
				qMin = (int)v;

			if ((int)v > qMax)
				qMax = (int)v;
		}

		TRACE(
			"QUANT range: %d .. %d\n",
			qMin,
			qMax);

		auto t3 = std::chrono::steady_clock::now();

		this->convertedChunks.push_back(
			WAV::ConvertedSample()
			.setConvertedChunk(quantized)
			.setScaleFactors(scaleFactors));

		auto t4 = std::chrono::steady_clock::now();
		/*
		TRACE(
			"hann=%lld ms mdct=%lld ms quant=%lld ms push=%lld ms\n",
			std::chrono::duration_cast<
			std::chrono::milliseconds>(t1 - t0).count(),
			std::chrono::duration_cast<
			std::chrono::milliseconds>(t2 - t1).count(),
			std::chrono::duration_cast<
			std::chrono::milliseconds>(t3 - t2).count(),
			std::chrono::duration_cast<
			std::chrono::milliseconds>(t4 - t3).count());
			*/
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
		for (int i = 0; i < windowSize; i++) {
			std::cout << window[i] << "\n";
		}

	}
	void applyHannWindow() {

		for (int i = 0; i < windowSize; i++) {
			window[i] *= sqrtf(hannWindow[i]);
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
	void MDCT(std::vector<float>& input, std::vector<float>& output)
	{
		int N = static_cast<int>(input.size()) / 2;

		if (cosTable.size() < N * (2 * N))
		{
			TRACE("ERROR: cosTable too small!\n");
			return;
		}

		output.resize(N);

		const float* in = input.data();


		for (int k = 0; k < N; k++)
		{
			float sum = 0.0f;

			const float* row =
				&cosTable[k * (2 * N)];

			for (int n = 0; n < 2 * N; n++)
			{
				sum += in[n] * row[n];
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
		int numChanels = 1; // we are converting to mono so sohuld always set it to 1
		MUC::MUCHeader header;
		header.sample_rate = wav.getSampleRate();
		header.channels = numChanels; 
		header.bits_per_sample = wav.getBitsPerSample();
		header.window_size = this->windowSize;
		header.total_frames = convertedChunks.size();
		header.total_samples = samplesLeft.size();
		header.bands_amount = bands.size();
		return header;
	}

};

