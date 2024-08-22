#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <chrono>

#define TIMERSTART(label)                                                           \
	std::chrono::time_point<std::chrono::high_resolution_clock> a##label, b##label; \
	a##label = std::chrono::high_resolution_clock::now();

#define TIMERSTOP(label)                                              \
	b##label = std::chrono::high_resolution_clock::now();             \
	std::chrono::duration<double> delta##label = b##label - a##label; \
	std::cout << "# elapsed time (" << #label << "): "                \
			  << delta##label.count() << "s" << std::endl;

namespace utils
{
	std::string now_time();
	std::vector<std::string> splitString(std::string line, std::string delims);
	std::unordered_map<std::string, std::string> parseArguments(int argc, char const *argv[], std::unordered_map<std::string, std::string> *);
	std::string modify_path(std::string old_path, int n,std::string text);
	bool validateExtension(std::string, std::string);
	
	template <typename T>
	std::vector<T>* joinVectors(std::vector<std::vector<T>> vecs);
};


#endif