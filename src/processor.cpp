#include "processor.h"

#include <unistd.h>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long previous_active_system_ticks_ = LinuxParser::ActiveJiffies();
  long previous_total_system_ticks_ = LinuxParser::Jiffies();

//   usleep(uInterval_);

  // long next_active_system_ticks = LinuxParser::ActiveJiffies();
  // long next_total_system_ticks = LinuxParser::Jiffies();

  return (float)previous_active_system_ticks_ /
         (float)previous_total_system_ticks_;
}