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
	std::vector<float> hannWindow;
	std::vector<Band> bands = config.getBands();
	int windowSize = config.getWindowSize();
	std::vector<WAV::ConvertedSample> convertedChunks;
	std::vector<float> cosTable = config.getCosTable();

	std::vector<float> decodedPCM;
	std::vector<int16_t> normalisedPCM;
public:

	WAV::WAVFile Decoder::decodeFile(MUC::MUCFile& muc)
	{
		fillHannWindowValues();
		WAV::WAVFile wav;
		convertedChunks = muc.getConvertedChunks();
		TRACE("Frames loaded = %zu\n", convertedChunks.size());
		decode(convertedChunks);
		TRACE("decodedPCM size = %zu\n", decodedPCM.size());
		TRACE("normalisedPCM size = %zu\n", normalisedPCM.size());
		wav.setHeader(muc.getSampleRate(), muc.getChannels(), muc.getBitsPerSample());
		wav.viewHeaders();
		wav.setPCM(normalisedPCM);
		TRACE("PCM bytes = %zu\n", wav.getData().size());
		return wav;
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

			int16_t normPCM = (int16_t)(s * 32767.0f);
			this->normalisedPCM[i] = normPCM;
		}

		return true;
	}

	std::vector<float> decodeFrame(WAV::ConvertedSample &chunk) {

		std::vector<float>IMDCTPrepared = this->dequantization(chunk, config.getQ());
		std::vector<float>result = this->IMDCT(IMDCTPrepared);
		for (int i = 0; i < windowSize; i++)
		{
			result[i] *= sqrtf(hannWindow[i]);
		}
		return result;
	}
	// goes second
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

			res[n] = sum * (2.0f / N);
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


	void fillHannWindowValues()
	{
		hannWindow.clear();

		float factor =
			2.0f * M_PI / (windowSize - 1);

		for (int i = 0; i < windowSize; i++)
		{
			float w =
				0.5f * (1.0f - cos(factor * i));

			hannWindow.push_back(w);
		}
	}


};

