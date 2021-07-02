#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  float current= LinuxParser::Jiffies();
  float active= LinuxParser::ActiveJiffies();
  return current/active; 
}
