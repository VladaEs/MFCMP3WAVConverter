#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

namespace WAV {

	class ConvertedSample
	{
	protected:
		const int chunkSize = 1024;
		std::vector<float> scaleFactors;
		std::vector<int> convertedChunk;
	public:
		ConvertedSample() { ; }
		ConvertedSample& setConvertedChunk(std::vector<int> & chunk) {
			this->convertedChunk = chunk;
			return *this;
		}
		ConvertedSample& setScaleFactors(std::vector<float> & factor) {
			this->scaleFactors = factor;
			return *this;
		}
		std::vector<float> getScaleFactors() {
			return this->scaleFactors;
		}
		std::vector<int> getConvertedChunk() {
			return this->convertedChunk;
		}


	};


}

