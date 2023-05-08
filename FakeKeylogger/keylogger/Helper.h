#pragma once


#include<ctime>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>

namespace Helper {
	template <class T>
	std::string ToString(const T&);

	struct DateTime {
		int Y, m, d, h, M, s;
		DateTime() {
			time_t ms;
			time(&ms);
			struct tm info;
			localtime_s(&info,&ms);
			Y = 1900 + info.tm_year;
			m = info.tm_mon+1;
			d = info.tm_mday;
			h = info.tm_hour;
			M = info.tm_min;
			s = info.tm_sec;
		}
		DateTime(int Y,int m,int d,int h,int M,int s): Y(Y),m(m),d(d),h(h),M(M),s(s) {}
		
		DateTime Now() const{
			return DateTime();
		}
		std::string GetDateString() const{
			return std::string(d < 10 ? "0" : "") + ToString(d) +
				std::string(m < 10 ? "-0" : "-") + ToString(m) + "-" +
				ToString(Y);
		}
		std::string getTimeString(const std::string &sep = "-") const {
			return std::string(h < 10 ? "0" : "") + ToString(h) + sep +
				std::string(M < 10 ? "0" : "") + ToString(M) + sep +
				std::string(s < 10 ? "0" : "") + ToString(s);
		}

		std::string getDateTimeString() const {
			return GetDateString() + "-" + getTimeString();
		}
	};
	template <class T>
	std::string ToString(const T &e) {
		std::ostringstream s;
		s << e;
		return s.str();
	}
	void WriteAppLog(const std::string &s) {
		std::ofstream fil("AppLog.txt", std::ios::app);
		fil << "[" << Helper::DateTime().getDateTimeString() << "]\n" << s << std::endl << "\n";
		fil.close();
	}
	std::vector<std::string> tokenize(std::string s, std::string del = "::")
	{
		std::vector<std::string> strs;
		int start = 0;
		int end = s.find(del);
		while (end != -1) {
			std::cout << s.substr(start, end - start) << std::endl;
			strs.push_back(s.substr(start, end - start));
			start = end + del.size();
			end = s.find(del, start);
		}
		std::cout << s.substr(start, end - start);
		strs.push_back(s.substr(start, end - start));
		return strs;
	}
}