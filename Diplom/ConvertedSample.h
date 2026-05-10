#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

namespace WAV {

	class ConvertedSample
	{
	protected:
		//const int chunkSize = 1024;
		std::vector<float> scaleFactors;
		std::vector<int8_t> convertedChunk;
	public:
		ConvertedSample() { ; }

		ConvertedSample(
			const std::vector<float>& scaleFactors,
			const std::vector<int8_t>& chunk
		) {
			setScaleFactors(scaleFactors);
			setConvertedChunk(chunk);
		}

		ConvertedSample& setConvertedChunk(const std::vector<int8_t> & chunk) {
			this->convertedChunk = chunk;
			return *this;
		}
		ConvertedSample& setScaleFactors(const std::vector<float> & factor) {
			this->scaleFactors = factor;
			return *this;
		}
		const std::vector<float>& getScaleFactors() {
			return this->scaleFactors;
		}
		int8_t getConvertedChunk(int i) {
			if (i > this->convertedChunk.size() || i < 0) return 0;
			return this->convertedChunk[i];
		}
		const std::vector<int8_t>& getConvertedChunk() {
			return this->convertedChunk;
		}



	};


}

