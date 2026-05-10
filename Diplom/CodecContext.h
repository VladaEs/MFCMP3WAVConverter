#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"

#pragma pack(push, 1)
struct Band {
	int start;
	int end;
};
#pragma pack(pop)

class CodecContext
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
	int Q = 50;

	const int windowSize = 2048;
	const char extension[4] = { 'M', 'U','X','3' };
	std::vector<std::vector<float>> cosTable;

public:

	CodecContext() {
		this->initCosTable(windowSize);
	}
	std::vector<Band>& getBands() {
		return bands;
	}
	const Band & getBand(int index) const {
		return bands.at(index);
	}

	void setBands(const std::vector<Band>& newBands) {
		bands = newBands;
	}

	int getBandsAmount() const {
		return static_cast<int>(bands.size());
	}


	int getWindowSize() const {
		return windowSize;
	}


	const char* getExtension() const {
		return extension;
	}
	std::vector<std::vector<float>> getCosTable() {
		
		return this->cosTable;
	}

	int getQ() {
		return this->Q;
	}

	CodecContext& setQ(int q) {
		this->Q = q;
		return *this;
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

