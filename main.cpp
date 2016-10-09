#include <iostream>
#include <cstdlib>
#include <cmath>

// =================================================================
// MAIN
// =================================================================

/*
The initial focus will be on processes, assumed to be resident in memory, waiting to use the CPU.

A process is defined as a program in execution.
Three states:
  READY: in the ready queue, ready to use the CPU
  RUNNING: actively using the CPU
  BLOCKED: blocked on I/O

Processes in the READY state reside in a simple queue, i.e. the ready queue. 
This queue is ordered based on a configurable CPU scheduling algorithm.
i.e., first-come-first-serve (FCFS), shortest job first (SJF), and round robin (RR). 

*/

/*
Simulation configuration

*/

/*
Input file

*/

/*
Required output

*/

/*
Handling errors

*/

int main(int argc, char* argv[]) {

  std::cout << "Hello World!" << std::endl;
  exit(1);

}