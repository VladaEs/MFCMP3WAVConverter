#pragma once
#include "pch.h"

#include "afxdialogex.h"
#include <string>
#include <vector>


class Helpers {
public:
	CString ConvertToCString(std::string str) {
		CString cstr(str.c_str());
		return cstr;
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
	std::string ConvertCharVectToString(std::vector<char> &data) {

		std::string res(data.size(), ' ');
		for (int i = 0; i < data.size(); i++) {
			res[i] = data[i];
		}
		return res;
	}
	std::string ConvertCStringToString(CString str) {
		CT2CA pszConvertedAnsiString(str);
		// construct a std::string using the LPCSTR input
		std::string strStd(pszConvertedAnsiString);
		return strStd;
	}
	std::vector<char> ConvertCStringToVectorChar(CString str) {
		const wchar_t* wstr = str.GetString();
		int len = str.GetLength();


		std::vector<char> vec(reinterpret_cast<const char*>(wstr),
			reinterpret_cast<const char*>(wstr + len));
		return vec;
	}


};

