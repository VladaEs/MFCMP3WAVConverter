#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <cstring>
#include <vector>
#include <afxwin.h>

#include "InvMusicFile.h"; 
#include "MUCFile.h";
#include "WAVFile.h";
#include "MP3.hpp"; 



class Music {
public:

	std::string path;
	std::string musicName;
	std::string extension;
	int timePaused = 0;
	double currentTimeMusic = 0;
	int index;
	double musicDuration;
	std::unique_ptr<InvMusicFile> file;
	bool musicLoaded = false;


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

	Music(const Music&) = delete;
	Music& operator=(const Music&) = delete;

	Music(Music&& other) noexcept
		: path(std::move(other.path)),
		musicName(std::move(other.musicName)),
		extension(std::move(other.extension)),
		timePaused(other.timePaused),
		currentTimeMusic(other.currentTimeMusic),
		index(other.index),
		musicDuration(other.musicDuration),
		file(std::move(other.file))
	{
	}
	Music& operator=(Music&& other) noexcept
	{
		if (this != &other)
		{
			path = std::move(other.path);

			musicName = std::move(other.musicName);

			extension = std::move(other.extension);

			timePaused = other.timePaused;

			currentTimeMusic = other.currentTimeMusic;

			index = other.index;

			musicDuration = other.musicDuration;

			file = std::move(other.file);
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
	bool setExtension(const std::string & ext) {
		this->extension = ext;
		return true;
	}
	std::string getExtension() {
		return this->extension;
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
	std::string getPlayableURL() {
		TRACE("\nPLAYABLE URL\n");
		TRACE(path.c_str());
		TRACE("\nendofLine");
		std::string URL = this->file->getPlayableURL(path);
		TRACE(URL.c_str());
		return URL;
	}
	std::string GetPath() {
		return this->path;
	}
	std::string GetMusicName() {
		return this->musicName;
	}
	std::vector<char> getTag(std::string tag) {
		return file->getTag(tag);
	}
	bool setTag(std::string tag, std::vector<char> data) {
		file->setTag(tag, data);
		return true;
	}
	bool loadMusic() {
		if (musicLoaded) return true;
		bool result = file->read(path.c_str());
		musicLoaded = true;
		return result;
	}
	void changeMusicName(std::string data) {
		file->setTag("TIT2", convertData(data));
	}
	std::vector<char> getMusicNameFile() {

		std::vector<char> name2 = file->getTag("TIT2");
		return file->getTag("TIT2");
	}

	std::vector<char> convertData(std::string data) {
		return std::vector<char>(data.begin(), data.end());
	}
	bool saveFile() {
		bool worked = file->write(path.c_str());
		return worked;
	}



private:




};


#endif