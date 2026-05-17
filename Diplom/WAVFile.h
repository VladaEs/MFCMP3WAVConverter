#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvertedSample.h"
#include "CodecContext.h"
#include "CodecContext.h"
#include "InvMusicFile.h"
#define M_PI           3.14159265358979323846

namespace WAV {
#pragma pack(push, 1)
	struct WavHeader
	{
		char chunk_id[4];
		//char chunk_id[4] = { 'R', 'I', 'F', 'F' };
		uint32_t chunk_size;
		char format[4];
		//char format[4] = { 'W', 'A', 'V','E' };

		//fmt subchunk
		char subchunk1_id[4];
		//char subchunk1_id[4] = { 'f', 'm', 't',' ' };
		uint32_t subchunk1_size;
		//uint32_t subchunk1_size = 16;
		//uint16_t subchunk1_format = 1; //PCM
		uint16_t subchunk1_format; //PCM
		uint16_t num_channels;
		//uint16_t num_channels = 2;

		uint32_t sample_rate;
		//uint32_t sample_rate = 44100;

		//uint32_t byte_rate = sample_rate * num_channels * (subchunk1_size / 8);
		uint32_t byte_rate;

		uint16_t block_align;
		//uint16_t block_align = num_channels * (subchunk1_size / 8);

		uint16_t bits_per_sample;
		//uint16_t bits_per_sample = 16;

		//data subchunk
		//char subchunk2_id[4] = { 'd','a','t','a' };
		char subchunk2_id[4];
		uint32_t subchunk2_size;


		unsigned int calcSize() {
		}
	};
#pragma pack(pop)







	class WAVFile :public InvMusicFile {
	private:

		
		CodecContext config;
		const int windowSize = config.getWindowSize();
		WAV::WavHeader header;
		int Q = 50; // степень квантования( качество сжатия)
		std::vector<char> data;
		std::vector<float> samplesLeft;
		std::vector<float> samplesRight;
		std::vector<float> window;

		std::vector<float> hannWindow;
		std::vector<ConvertedSample> convertedChunks;
		std::string fileName;
		//std::vector<std::vector<float>> cosTable = config.getCosTable();
		const char extension[4] = { 'M', 'U','X','3' };
		int currPosition = 0;
		bool headerSet = false;

	public:

		WAVFile() {
			window.resize(windowSize);
			hannWindow.resize(windowSize);
		}

		WAVFile(std::vector<float> decodedPCM) : WAVFile() {
			
		}
		int getQ() {
			return this->Q;
		}
		int setQ(int newQ) {
			this->Q = newQ;
			return this->Q;
		}
		bool read(const char* path) {
			// open file
			std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

			if (!file.is_open()) {
				return false;
			}
			// get the file size
			std::ifstream::pos_type fileSize = file.tellg();
			if (fileSize < 0) {
				return false;
			}
			// move cursos to beg. 
			file.seekg(0, std::ios::beg);

			// read data to buffer
			data = std::vector<char>(fileSize); // init vector with empty cells 
			file.read(&data[0], fileSize);

			// copy first 44 bytes for the WAV header
			std::memcpy(&header, &data[0], sizeof(WAV::WavHeader));
			this->headerSet = true;
			for (int i = 0; i < sizeof(WAV::WavHeader); i++) {
				data.erase(data.begin());
			}


			data.shrink_to_fit();
			std::cout << "Map Started\n";
			//this->initCosTable(windowSize / 2);
			std::cout << "\nMap Ended \n";
			std::cout << "Raw Data started \n";
			//this->processRawData();
			std::cout << "Raw Data ended\n";
			file.close();
			return true;
		}

		bool write(const char* path) {
			this->recalcHeaderSize();

			std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::ate);
			if (!file.is_open()) {
				return false;
			}
			//file.write(reinterpret_cast<char *>(&header), sizeof(WAV::CompressedWavHeader));

			file.write(&data[0], data.size());
		}


		WAV::WAVFile & setSaveFileName(std::string name) {
			this->fileName = name;
			return *this;
		}
		std::vector<char> getData() {
			return this->data;
		}

		WAV::WavHeader & getHeader() {
			return this->header;
		}
		void viewHeaders() {
			std::cout << header.bits_per_sample << "\n";
			std::cout << header.block_align << "\n";
			std::cout << header.byte_rate << "\n";
			std::cout << header.chunk_id << "\n";
			std::cout << header.chunk_size << "\n";
			std::cout << header.format << "\n";
			std::cout << header.num_channels << "\n";
			std::cout << header.sample_rate << "\n";
			std::cout << header.subchunk1_format << "\n";
			std::cout << header.subchunk1_id << "\n";
			std::cout << header.subchunk1_size << "\n";
			std::cout << header.subchunk2_id << "\n";
			std::cout << header.subchunk2_size << "\n";
		}
		WAVFile* recalcHeaderSize() {

			return this;
		}
		
		bool writeDataToFile() {
			//CompressedWavHeader newWAVheader = this->prepareCustomHeader();
			return false;
		}


		// header Getters

		//-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			uint32_t getChunkSize() const {
				return header.chunk_size;
			}

			uint32_t getSampleRate() const {
				return header.sample_rate;
			}

			uint16_t getNumChannels() const {
				return header.num_channels;
			}

			uint16_t getBitsPerSample() const {
				return header.bits_per_sample;
			}

			uint32_t getByteRate() const {
				return header.byte_rate;
			}

			uint16_t getBlockAlign() const {
				return header.block_align;
			}

			uint32_t getDataSize() const {
				return header.subchunk2_size;
			}

			uint16_t getAudioFormat() const {
				return header.subchunk1_format;
			}

			uint32_t getFmtChunkSize() const {
				return header.subchunk1_size;
			}
		//-=-=-=-=-=-=-=-=-=-=-=-=-=-=


	};
}

