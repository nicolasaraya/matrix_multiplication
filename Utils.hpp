#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <map>
#include <chrono>

#include <cstddef>
#include <atomic>
#include <iostream>
#include <mutex>

#if defined(BITS64)
  typedef long long int Int;
  typedef unsigned long long int uInt;
  #define UINTMAX ULONG_MAX
  #define INTMAX LONG_MAX
#else
  typedef int Int;
  typedef unsigned int uInt;
  #define UINTMAX 4294967295
  #define INTMAX 2147483647
#endif


#define TIMERSTART(label)             \
  std::chrono::time_point<std::chrono::high_resolution_clock> a##label, b##label; \
  a##label = std::chrono::high_resolution_clock::now();

#define TIMERSTOP(label)                                            \
  b##label = std::chrono::high_resolution_clock::now();             \
  std::chrono::duration<double> delta##label = b##label - a##label; \
  std::cout << "# elapsed time (" << #label << "): "                \
            << delta##label.count() << "s" << std::endl;


namespace utils
{
  std::string now_time();
  std::vector<std::string> splitString(std::string line, std::string delims);
  #if 0
  std::unordered_map<std::string, std::string> parseArguments(int argc, char const *argv[], std::unordered_map<std::string, std::string> *);
  #endif
  std::string modify_path(std::string old_path, int n, std::string text);
  std::string modify_path(std::string old_path, std::string text);
  bool validateExtension(std::string, std::string);

  class MemoryTracker {
  public:
      static void AddAllocation(std::size_t size);
      static void RemoveAllocation(std::size_t size);
      static void Report();

  private:
      static std::atomic<std::size_t> totalAllocated;
      static std::atomic<std::size_t> totalFreed;
      static std::atomic<std::size_t> currentUsage;
      static std::atomic<std::size_t> peakUsage;
      static std::mutex mutex;
  };

};




#endif