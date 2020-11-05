#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { Process::pid_ = pid; }

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
// Calculation from:
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() {
  // Total time spent for process
  // total_time = ActiveJiffies= utime + stime + cutime + cstime
  double proc_total_time_ticks = double(LinuxParser::ActiveJiffies(pid_));

  // Total elapsed time since process started
  // seconds = uptime - (starttime/ Hertz)
  double Hertz = double(sysconf(_SC_CLK_TCK));
  double system_uptime = double(LinuxParser::UpTime());
  double proc_startTime_ticks = double(LinuxParser::UpTime(pid_));
  double seconds = system_uptime - (proc_startTime_ticks / Hertz);

  // Finally we calculate the CPU usage percentage
  // cpu_usage = 100 * (total_time / Hertz) / seconds)
  float cpu_usage = 100.0 * ((proc_total_time_ticks / Hertz) / seconds);

  return cpu_usage;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return stol(LinuxParser::Ram(a.pid_)) < stol(LinuxParser::Ram(pid_));
}