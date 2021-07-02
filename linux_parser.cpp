#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::stol;
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
float LinuxParser::MemoryUtilization() { 
  float memTotal=0, memFree=0;
  string line,key;
  std::ifstream stream(kProcDirectory+kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::remove(line.begin(), line.end(), ' ');
      std::istringstream linestream(line);
      while(linestream>>key){
        if(key== "MemTotal"){
          linestream>>memTotal;
        }
        if(key=="MemFree"){
          linestream>>memFree;
          break;
        }
      }
    }
  }
  
  return (memTotal-memFree)/memTotal; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, value;
  long time=0;
  std::ifstream stream(kProcDirectory+kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>>value;
    time=stol(value);
  }
  return time; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return ActiveJiffies()+IdleJiffies(); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, val;
  vector<string> v;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream>>val){
      v.push_back(val);
    }
  } 
  if(v.size()>=21){
    return (stol(v[13])+stol(v[14])+stol(v[15])+stol(v[16]));
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto stats=LinuxParser::CpuUtilization();
  long user=stol(stats[LinuxParser::kUser_]);
  long nice = stol(stats[LinuxParser::kNice_]);
  long system = stol(stats[LinuxParser::kSystem_]);
  long idle = stol(stats[LinuxParser::kIdle_]);
  long iowait = stol(stats[LinuxParser::kIOwait_]);
  long irq = stol(stats[LinuxParser::kIRQ_]);
  long softirq = stol(stats[LinuxParser::kSoftIRQ_]);
  long steal = stol(stats[LinuxParser::kSteal_]);
  return (user+nice+system+idle+iowait+irq+softirq+steal);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto stats=LinuxParser::CpuUtilization();
  long idle= stol(stats[LinuxParser::kIdle_]);
  long iowait = stol(stats[LinuxParser::kIOwait_]);
  return (idle+iowait); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, val;
  vector<string> v;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream>>val){
      v.push_back(val);
    }
  } 
  return v; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int process;
  string key,line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
     std::istringstream linestream(line);
     while(linestream>>key>>process){
       if(key=="processes"){
         return process;
       }
     }
    }
  } 
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int process=0;
  string key,line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
     std::istringstream linestream(line);
     while(linestream>>key>>process){
       if(key=="procs_running"){
         return process;
       }
     }
    }
  } 
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line,key,strVal;
  long val;
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
        std::istringstream linestream(line);
        while(linestream>>key){
          if(key=="VmSize:"){
            linestream>>val;
            val/=1000;
            strVal=to_string(val);
            break;
          }
        }
    }
    return strVal;
  }
  return line; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, uid;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream>>key;
      if(key=="Uid:"){
        linestream>>uid;
        break;
      }
    }
  }
  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string key= Uid(pid);
  string user, line, idStr;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream>>user>>idStr){
        if(idStr==key){
          return user;
        }
      }
    }
  }
  return user; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line,val;
  vector<string> vals;
  long time=0;
  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline (stream,line);
    std::istringstream linestream(line);
    while(linestream>>val){
      vals.push_back(val);
    }  
  }
  if(vals.size()>=21){
    time=stol(vals[21]);
    return UpTime()-(time/sysconf(_SC_CLK_TCK));
  }
  return 0; 
}
