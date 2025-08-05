#include "Utils.hpp"

#include <atomic>
#include <mutex>

std::atomic<std::size_t> utils::MemoryTracker::totalAllocated{0};
std::atomic<std::size_t> utils::MemoryTracker::totalFreed{0};
std::atomic<std::size_t> utils::MemoryTracker::currentUsage{0};
std::atomic<std::size_t> utils::MemoryTracker::peakUsage{0};
std::mutex utils::MemoryTracker::mutex;

std::string utils::now_time()
{
  std::time_t t = std::time(0); // get time now
  std::tm *t_now = std::localtime(&t);
  std::string now = std::to_string(t_now->tm_year + 1900) + '-' + std::to_string(t_now->tm_mon + 1) + '-' + std::to_string(t_now->tm_mday) + "-" + std::to_string(t_now->tm_hour) + std::to_string(t_now->tm_min) + std::to_string(t_now->tm_sec);
  return now;
}

std::vector<std::string> utils::splitString(std::string line, std::string delims)
{
  std::string::size_type bi, ei;
  std::vector<std::string> words;
  bi = line.find_first_not_of(delims);
  while (bi != std::string::npos)
  {
    ei = line.find_first_of(delims, bi);
    if (ei == std::string::npos)
        ei = line.length();
    std::string aux = line.substr(bi, ei - bi);
    words.push_back(aux.c_str());
    bi = line.find_first_not_of(delims, ei);
  }
  return words;
}

#if 0
std::unordered_map<std::string, std::string> utils::parseArguments(int argc, char const *argv[], std::unordered_map<std::string, std::string> *arguments)
{
  const std::string prefix = "--";

  for (int i = 1; i < argc - 1; i++){
    auto arg = std::string(argv[i]);
    if (arg.front() != '-') {
      continue;
    }

    if(arg.front() == '-') arg.erase(arg.begin()); 
    if(arg.front() == '-') arg.erase(arg.begin()); 

    auto f = arguments->find(arg);
    if (f == arguments->end()) {
      std::cout << "No se encuentra el argumento: " << prefix + std::string(arg) << std::endl;
      continue;
    }
    arguments->at(arg) = std::string(argv[i + 1]);
    //std::cout << arg << ", " << arguments->at(arg) << std::endl;
  }
  return *arguments;
}
#endif

std::string utils::modify_path(std::string old_path, int n, std::string text)
{
  std::string new_path = old_path;
  while(n--) {
    new_path.pop_back();
  }
  new_path += text;
  return new_path;
}

std::string utils::modify_path(std::string old_path, std::string text)
{
  std::string new_path = old_path;
  while(new_path.back() != '.') {
    new_path.pop_back();
  }
  new_path.pop_back();
  new_path += text;
  return new_path;
}

bool utils::validateExtension(std::string path, std::string extension)
{
  if(path.find("." + extension) != std::string::npos) {
		return true;
	}
  return false;
}



void utils::MemoryTracker::AddAllocation(std::size_t size) {
    totalAllocated += size;
    currentUsage += size;
    std::lock_guard<std::mutex> lock(mutex);
    if (currentUsage > peakUsage) {
        peakUsage = currentUsage.load();
    }
}

void utils::MemoryTracker::RemoveAllocation(std::size_t size) {
    totalFreed += size;
    currentUsage -= size;
}

void utils::MemoryTracker::Report() {
    std::cout << "----- Memory Report -----\n";
    std::cout << "Total allocated: " << totalAllocated.load() << " bytes\n";
    std::cout << "Total freed:     " << totalFreed.load() << " bytes\n";
    std::cout << "Current usage:   " << currentUsage.load() << " bytes\n";
    std::cout << "Peak usage:      " << peakUsage.load() << " bytes\n";
    std::cout << "-------------------------\n";
}
