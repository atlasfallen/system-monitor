#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  // long uInterval_{1000}; // microsecond interval
  long previous_active_system_ticks_;
  long previous_total_system_ticks_;
};

#endif