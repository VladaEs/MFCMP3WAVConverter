#pragma once

#include <vector>
#include <cstring>


class InvMusicFile {
public:
	virtual bool read(const char* path) = 0;
	virtual bool write(const char* path) = 0;
	virtual std::vector<char> getTag(std::string tag) {
		return {};
	}

	virtual void setTag(std::string tag, std::vector<char> data) {
		return ;
	}

	virtual ~InvMusicFile() = default;
	virtual bool play() = 0 ;
	virtual bool pause() = 0;
	virtual std::string getPlayableURL (std::string path) = 0;
	
};

