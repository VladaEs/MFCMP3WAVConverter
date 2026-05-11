#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "ConvertedSample.h";


namespace MUC {


#pragma pack(push, 1)

	struct MUCHeader {
		char codec_tag[4] = { 'M', 'U', 'C', '1' };

		char window_name[8] = "HANN";

		int version = 1;

		uint32_t sample_rate = 0;
		uint16_t channels = 0;
		uint16_t bits_per_sample = 0;

		uint16_t window_size = 0;
		uint16_t bands_amount = 0;
		uint16_t quant_levels = 50;

		uint32_t total_frames = 0;
		uint32_t total_samples = 0;

	};

#pragma pack(pop)
	class MUCFile
	{
	protected:
		std::vector<WAV::ConvertedSample> convertedData;
		bool headerSet = false;
		MUCHeader header;
		std::vector<char> data;

	public:

		std::vector<WAV::ConvertedSample> setConvertedData(std::vector<WAV::ConvertedSample>& chunks) {
			this->convertedData = chunks;
		}


		bool write(const char *path) {
			std::ofstream file(path, std::ios::out | std::ios::binary);
			if (!file.is_open()) {
				return false;
			}

			file.write(reinterpret_cast<const char*>(&header), sizeof(MUCHeader));
			for (int i = 0; i < convertedData.size(); i++) {
				file.write(reinterpret_cast<const char*>(convertedData[i].getScaleFactors().data()), convertedData[i].getScaleFactors().size() * sizeof(float));
				file.write(reinterpret_cast<const char*>(convertedData[i].getConvertedChunk().data()), convertedData[i].getConvertedChunk().size() * sizeof(int8_t));
			}

			return true;
		}
		bool read(const char* path) {
			std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

			if (!file.is_open()) {
				return false;
			}
			std::ifstream::pos_type fileSize = file.tellg();

			// move cursor to beginning
			file.seekg(0, std::ios::beg);

			// read data to buffer
			data = std::vector<char>(fileSize);
			file.read(&data[0], fileSize);

			memcpy(&header, &data[0], sizeof(MUCHeader));
			/*
			for (int i = 0, size = sizeof(MUCHeader); i < size; i++) {
				data.erase(data.begin());
			}
			*/
			// do not delete header from array, just offset it

			int MDCTSize = this->getWindowSize() / 2;
			std::vector<float> scaleFactors(this->getBandsAmount());
			std::vector<int8_t> convertedChunk(MDCTSize);

			int offset = sizeof(MUCHeader);

			for (int i = 0; i < this->getTotalFrames(); i++) {
				memcpy(scaleFactors.data(), &data[offset], scaleFactors.size() * sizeof(float));
				offset += scaleFactors.size() * sizeof(float);
				memcpy(convertedChunk.data(), &data[offset], convertedChunk.size() * sizeof(int8_t));
				offset += convertedChunk.size() * sizeof(int8_t);
				WAV::ConvertedSample temp(scaleFactors, convertedChunk);
				convertedData.push_back(temp);
			}

			return true;


		}

		MUCHeader setCustomHeader(MUCHeader& headerInput) {
			this->header = headerInput;
		}


		std::vector<WAV::ConvertedSample> & getConvertedChunks() {
			return this->convertedData;
		}






























		const MUCHeader& getHeader() const {
			return header;
		}

		uint32_t getSampleRate() const {
			return header.sample_rate;
		}

		uint16_t getChannels() const {
			return header.channels;
		}

		uint16_t getBitsPerSample() const {
			return header.bits_per_sample;
		}

		uint16_t getWindowSize() const {
			return header.window_size;
		}

		uint16_t getQuantLevels() const {
			return header.quant_levels;
		}
		uint16_t getBandsAmount() const {
			return header.bands_amount;
		}

		uint32_t getTotalFrames() const {
			return header.total_frames;
		}

		uint32_t getTotalSamples() const {
			return header.total_samples;
		}

		int getVersion() const {
			return header.version;
		}

		// =========================
		// Setters
		// =========================
		void setBandsAmount(uint16_t amount) {
			header.bands_amount = amount;
		}
		void setSampleRate(uint32_t rate) {
			header.sample_rate = rate;
		}

		void setChannels(uint16_t ch) {
			header.channels = ch;
		}

		void setBitsPerSample(uint16_t bits) {
			header.bits_per_sample = bits;
		}

		void setWindowSize(uint16_t size) {
			header.window_size = size;
		}

		void setQuantLevels(uint16_t levels) {
			header.quant_levels = levels;
		}

		void setTotalFrames(uint32_t frames) {
			header.total_frames = frames;
		}

		void setTotalSamples(uint32_t samples) {
			header.total_samples = samples;
		}

		void setVersion(int ver) {
			header.version = ver;
		}

		uint32_t calcHeaderSize() const {
			return sizeof(MUCHeader);
		}
	};

}

