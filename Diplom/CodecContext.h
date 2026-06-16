#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"

#define M_PI       3.14159265358979323846

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

	static constexpr int windowSize = 2048;
	const char extension[4] = { 'M', 'U','X','3' };
	std::vector<float> cosTable;

	CodecContext() {
		this->initCosTable(windowSize / 2);
	}

public:
	static CodecContext& Instance()
	{
		static CodecContext instance;
		return instance;
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
	std::vector<float>& getCosTable() {
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
		cosTable.clear();
		cosTable.resize(N * 2 * N);
		for (int k = 0; k < N; k++)
		{
			for (int n = 0; n < 2 * N; n++)
			{
				float angle = M_PI / N *
					(n + 0.5f + N / 2.0f) *
					(k + 0.5f);
				cosTable[k * (2 * N) + n] = cosf(angle);
			}
		}
	}

};

