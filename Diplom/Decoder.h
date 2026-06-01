#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"
#include "MUCFile.h"
#include "WAVFile.h"
#include "CodecContext.h"

#define M_PI           3.14159265358979323846

class Decoder
{

private: 
	CodecContext& config = CodecContext::Instance();
	
	std::vector<Band> bands = config.getBands();
	int windowSize = config.getWindowSize();
	std::vector<WAV::ConvertedSample> convertedChunks;
	std::vector<std::vector<float>> cosTable = config.getCosTable();

	std::vector<float> decodedPCM;
	std::vector<int16_t> normalisedPCM;
public:

	MUC::MUCFile Decode(MUC::MUCFile& muc) {
		WAV::WAVFile wav;
		convertedChunks = muc.getConvertedChunks();

	}

	bool decode(std::vector<WAV::ConvertedSample> &chunks) {
		int hopSize = windowSize / 2;
		decodedPCM.resize( chunks.size() * hopSize + hopSize );
		for (int i = 0; i < chunks.size(); i++) {
			std::vector<float> frame = decodeFrame(chunks[i]);
			int pos = i * hopSize;

			for (int i = 0; i < frame.size(); i++) {
				decodedPCM[pos + i] += frame[i];
			}
		}
		this->normaliseData(this->decodedPCM);
		return true;
	}


	bool normaliseData(std::vector<float> &pcm) {
		normalisedPCM.resize(pcm.size());
		for (int i = 0; i < pcm.size(); i++) {
			float s = pcm[i];

			if (s > 1.0f) s = 1.0f;
			if (s < -1.0f) s = -1.0f;

			int16_t normPCM = (int16_t)(pcm[i] * 32767.0f);
			this->normalisedPCM[i] = normPCM;
		}

		return true;
	}

	std::vector<float> decodeFrame(WAV::ConvertedSample &chunk) {
		std::vector<float>IMDCTPrepared = this->dequantization(chunk, config.getQ());
		std::vector<float>result = this->IMDCT(IMDCTPrepared);
		return result;
	}
	// goes second
	std::vector<float> IMDCT(std::vector<float> &input){
		std::vector<float> res(windowSize);
		for (int n = 0; n < windowSize; n++) {
			float sum = 0;

			for (int k = 0; k < input.size(); k++) {
				sum += input[k] * this->cosTable[k][n];
			}
			res[n] = sum;
		}

		return res;
	}
	// goes first from [-127, 127] to float
	std::vector<float> dequantization(WAV::ConvertedSample sample, int Q) {
		std::vector<float> scaleFactor = sample.getScaleFactors();
		std::vector<float> res(sample.getConvertedChunk().size());
		std::vector<int8_t> chunks = sample.getConvertedChunk();
		for (int b = 0; b < bands.size(); b++) {
			float scale = scaleFactor[b];
			int start = bands[b].start;
			int end = bands[b].end;
			if (end > chunks.size()) {
				end = chunks.size();
			}
			for (int i = start; i < end; i++) {
				res[i] = ((chunks[i] / (float)Q)* scale);
			}
		}

		return res;
	}

};

