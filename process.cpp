#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
using std::stol;
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
  pid_=pid;
  command_=LinuxParser::Command(pid);
  string ram=LinuxParser::Ram(pid);
  ram_=stol(ram);
  user_=LinuxParser::User(pid);
  upTime_=LinuxParser::UpTime(pid);
  cpu_= float(LinuxParser::ActiveJiffies(pid))/float(LinuxParser::UpTime()-upTime_);
  
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpu_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return to_string(ram_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return upTime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return cpu_ < a.cpu_; 
}
