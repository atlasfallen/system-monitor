#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
// How to calculate memory utilization @
// https://access.redhat.com/solutions/406773
// For future improvements, how to deal with multiple
// processors is @
// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
float LinuxParser::MemoryUtilization() {
  string key, value;
  float memTotal, memFree;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:")
        memTotal = std::stof(value);
      else if (key == "MemFree:") {
        memFree = std::stof(value);
        break;
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// DONE: Read and return the system uptime
// Man page for uptime @
// https://man7.org/linux/man-pages/man5/proc.5.html
long LinuxParser::UpTime() {
  string systemUpTime_str;
  long systemUpTime{0};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> systemUpTime_str;
    systemUpTime = std::stol(systemUpTime_str);
  }
  return systemUpTime;
}

// DONE: Read and return the number of jiffies for the system
// Instruction on how to calculate Jiffies @
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::Jiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  // Does not include kGuest_ and kGuestNice_ because they are
  // already added into kUser_ and kNice (as described in link above)
  return stol(cpuUtilization[kUser_]) + stol(cpuUtilization[kNice_]) +
         stol(cpuUtilization[kSystem_]) + stol(cpuUtilization[kIdle_]) +
         stol(cpuUtilization[kIOwait_]) + stol(cpuUtilization[kIRQ_]) +
         stol(cpuUtilization[kSteal_]);
}

// DONE: Read and return the number of active jiffies for a PID
// In /proc/[pid]/stat file add ammount of time in user mode,
// kernel mode, children from user, and children wait time
// (utime + stime + cutime + cstime)
// calculation decribed @
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) {
  string line, clock_ticks;
  long total_clock_ticks{0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int count{1};
    while (count <= 17 && linestream >> clock_ticks) {
      if (count >= 14 && count <= 17) total_clock_ticks += stol(clock_ticks);
      count++;
    }
  }
  return total_clock_ticks;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  return stol(cpuUtilization[kIdle_]) + stol(cpuUtilization[kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string value;
  vector<string> cpuUtilization{};
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    // Aggregate CPU is on first line
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // discard first value
    while (linestream >> value) {
      cpuUtilization.push_back(value);
    }
  }
  return cpuUtilization;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") return stoi(value);
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "proc_running") return stoi(value);
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmdline{""};
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmdline);
    return cmdline;
  }
  return cmdline;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// DONE: Read and return the uptime of a process
// Man page on proc/[pid]/ files
// https://man7.org/linux/man-pages/man5/proc.5.html
// Stackoverflow on how to use data in stat file @
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::UpTime(int pid) {
  string line, starttime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int count{1};
    while (count <= 22 && linestream >> starttime) {
      // Return elspsed time in seconds since process started
      if (count == 22) return stol(starttime) / sysconf(_SC_CLK_TCK);
      count++;
    }
  }
  return 0;
}