#include "pch.h"
#include "MUCFile.h"
#include "Decoder.h"
#include <ctime>
std::string MUC::MUCFile::getPlayableURL(std::string path)
{
	TRACE("PLayableURLMUC");
	std::filesystem::path p(path);

	std::string directory = p.parent_path().string();

	Decoder decoder;

	WAV::WAVFile decoded = decoder.decodeFile(*this);
	time_t timestamp;
	time(&timestamp);

	tm localTime;
	localtime_s(&localTime, &timestamp);

	char buffer[64];

	strftime( buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S",&localTime);

	std::string fileName = std::string(buffer) + ".wav";

	std::string filepath = (std::filesystem::path(directory) / fileName).string();
	
	TRACE("\n\nFilePath\n\n");
	TRACE(filepath.c_str());
	TRACE("\n\nFilePath\n\n");
	decoded.write(filepath.c_str());

	return filepath;
}