#include "pch.h"
#include "MUCFile.h"
#include "Decoder.h"

std::string MUC::MUCFile::getPlayableURL(std::string path)
{
	TRACE("PLayableURLMUC");
	std::filesystem::path p(path);

	std::string directory = p.parent_path().string();

	Decoder decoder;

	WAV::WAVFile decoded = decoder.decodeFile(*this);

	std::string filepath = (std::filesystem::path(directory) /"temp.wav").string();
	TRACE("\n\nFilePath\n\n");
	TRACE(filepath.c_str());
	TRACE("\n\nFilePath\n\n");
	decoded.write(filepath.c_str());

	return filepath;
}