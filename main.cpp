#include <iostream>
#include <cstdlib>
#include <cmath>

// =================================================================
// MAIN
// =================================================================

/*
In this first project, you will implement a rudimentary simulation of an operating system. 
Theinitial focus will be on processes, assumed to be resident in memory, waiting to use the CPU.
*/

/*
A process is defined as a program in execution.
Three states:
  READY: in the ready queue, ready to use the CPU
  RUNNING: actively using the CPU
  BLOCKED: blocked on I/O

Processes in the READY state reside in a simple queue, i.e. the ready queue. 
This queue is ordered based on a configurable CPU scheduling algorithm.
i.e., first-come-first-serve (FCFS), shortest job first (SJF), and round robin (RR). 
Note that all three algorithms will be applied to the same set of simulated processes.

Once a process reaches the front of the queue (and the CPU is free to accept a process), the given
process enters the RUNNING state and executes its current CPU burst.

After the CPU burst is completed, the process enters the BLOCKED state, performing some sort
of I/O operation (e.g., writing results to the terminal or a file, interacting with the user, etc.). 
Once the I/O operation completes, the process returns to the READY state and is added to the ready
queue, with its position within the queue based on each given CPU scheduling algorithm.

First-Come-First-Serve
The FCFS algorithm is a non-preemptive algorithm in which processes line up in the ready queue, awaiting the CPU.

Shortest Job First
The SJF algorithm is also a non-preemptive algorithm. In SJF, processes are stored in the ready
queue in order of priority based on their CPU burst times. More specifically, the process with the
shortest CPU burst time will be the next process executed by the CPU.

Round Robin (RR)
The RR algorithm is essentially the FCFS algorithm with a predefined time slice t_slice. Each
process is given t_slice amount of time to complete its CPU burst. If this time slice expires, the
process is preempted and added to the end of the ready queue. If a process completes its CPU burst
before a time slice expiration, the next process on the ready queue is immediately context-switched
into the CPU.
For your simulation, if a preemption occurs and there are no other processes on the ready queue,
do not perform a context switch. For example, if process A is using the CPU and the ready queue
is empty, if process A is preempted by a time slice expiration, do not context-switch process A back
to the empty queue. Instead, keep process A running with the CPU and do not count this as a
context switch. In other words, when the time slice expires, check the queue to determine if a
context switch should occur.
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