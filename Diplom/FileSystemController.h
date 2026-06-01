#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <filesystem>
#include <direct.h>

namespace FSC {

class FileSystemController {

	public:
		std::string rootDir;
		std::vector<std::string> extensions = {".mp3", ".wav" , ".muc"};
		std::vector<std::string> filesDir;

		FileSystemController() { ; }
		FileSystemController(std::string rootDir) {
			this->rootDir = rootDir;
		}

		FileSystemController& addExtension(std::string ext) {
			if (ext[0] != '.') {
				std::string newExt = "." + ext;
				this->extensions.push_back(ext);
				return *this;
			}
			this->extensions.push_back(ext);
			return *this;
		}
		FileSystemController& SetRootDir(std::string dirName) {
			this->rootDir = dirName;
			LoadFiles();
			return *this;
		}

		std::vector<std::string> LoadFiles() {
			this->filesDir.clear();
			//рекурсивна функція
			get_files(filesDir, rootDir);
			return filesDir;
		}
		std::vector<std::string>& GetPathList() {
			return this->filesDir;
		}

	protected:
		void get_files(std::vector<std::string> &paths, const std::string& currentPath) {
			for (const auto& file : std::filesystem::directory_iterator(currentPath)) {
				if (file.is_directory()) {
					get_files(paths, file.path().string());
				}
				else {
					if(this->inArray<std::string>(file.path().extension().string(), this->extensions)){
						paths.push_back(file.path().string());
					}
				}
			}
		}


		template <typename T>
		bool inArray(const T needle, const std::vector<T>& haystack) {
			for (int i = 0; i < haystack.size(); i++) {
				if (needle == haystack[i]) {
					return true;
				}
			}
			return false;
		}
	};



}

