#include "processor.h"

#include <unistd.h>

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  active_system_ticks_ = LinuxParser::ActiveJiffies();
  total_system_ticks_ = LinuxParser::Jiffies();
  return (float)active_system_ticks_ / (float)total_system_ticks_;
}