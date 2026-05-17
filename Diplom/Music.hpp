#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <cstring>
#include <vector>
#include <afxwin.h>

#include "mp3.hpp"



class Music {
public:

	std::string path;
	std::string musicName;
	int timePaused = 0;
	double currentTimeMusic = 0;
	int index;
	double musicDuration;
	MP3 file;




	Music(std::string p, std::string name) {
		this->path = p;
		this->musicName = name;

	}
	Music(CWnd* parent, std::string p, std::string name, int i) {
		this->path = p;
		this->musicName = name;
		this->index = i;

	}
	Music(std::string p, std::string name, int i) {
		this->path = p;
		this->musicName = name;
		this->index = i;

	}
	Music() { ; }

	Music(const Music& other)
		: path(other.path), musicName(other.musicName), timePaused(other.timePaused), index(other.index) {}
	Music& operator=(const Music& other) {
		if (this != &other) {

			path = other.path;
			musicName = other.musicName;
			timePaused = other.timePaused;
			index = other.index;

		}
		return *this;
	}
	std::string cleanTag(std::vector<char>& data) {
		std::string res;
		for (int i = 0; i < data.size(); i++) {
			if (data[i] >= 32 && data[i] <= 128) {
				res += data[i];
			}
		}
		return res;
	}



	Music & SetPath(std::string path) {
		this->path = path;
		return *this;
	}
	Music & SetMusicName(std::string name) {
		this->musicName = name;
		return *this;
	}
	Music & SetTimePaused(int t) {
		this->timePaused = t;
		return *this;
	}


	double getMusicDuration() {
		return this->musicDuration;
	}
	Music & setMusicDuration(double sec) {
		this->musicDuration = sec;
		return *this;
	}

	double GetCurrentMusicTime() {
		return this->currentTimeMusic;
	}
	Music& SetCurrentMusicTime(double time) {
		this->currentTimeMusic = time;
		return *this;
	}
	int GetTimePaused() {
		return this->timePaused;
	}
	std::string GetPath() {
		return this->path;
	}
	std::string GetMusicName() {
		return this->musicName;
	}
	std::vector<char> getTag(std::string tag) {
		return file.getTag(tag);
	}
	bool setTag(std::string tag, std::vector<char> data) {
		file.setTag(tag, data);
		return true;
	}
	bool loadMusic() {
		return file.read(path.c_str());
	}
	void changeMusicName(std::string data) {
		file.setTag("TIT2", convertData(data));
	}
	std::vector<char> getMusicNameFile() {

		std::vector<char> name2 = file.getTag("TIT2");
		return file.getTag("TIT2");
	}

	std::vector<char> convertData(std::string data) {
		return std::vector<char>(data.begin(), data.end());
	}
	bool saveFile() {
		bool worked = file.write(path.c_str());
		return worked;
	}



private:




};


#endif