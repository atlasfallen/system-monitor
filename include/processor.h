#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // Declare any necessary private members
 private:
  long active_system_ticks_;
  long total_system_ticks_;
};

#endif