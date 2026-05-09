#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "ConvertedSample.h";


namespace MUC {


#pragma pack(push, 1)

	struct MUCHeader {
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
	class MUCFile
	{
	protected:
		std::vector<WAV::ConvertedSample> convertedData;

	public:
		std::vector<WAV::ConvertedSample> setConvertedData(std::vector<WAV::ConvertedSample>& chunks) {
			this->convertedData = chunks;
		}


		bool write() {

		}


		MUCHeader prepareCustomHeader() {
			MUCHeader header;
			int totalSamples = this->samplesLeft.size();
			int hopSize = windowSize / 2;
			int totalFrames = (totalSamples + hopSize - 1) / hopSize;
			header.sample_rate = this->header.sample_rate;
			header.channels = this->header.num_channels;
			header.bits_per_sample = this->header.bits_per_sample;
			header.window_size = this->windowSize;
			//header.mdct_size = this->windowSize / 2;
			header.quant_levels = this->Q;
			header.total_frames = this->convertedChunks.size();
			return header;
		}

	};

}

