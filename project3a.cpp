// Erica Oytas
// CS 370: Project 3
// FALL17

// The purpose of this project is to provide insight into the Linux Processs Scheduling Algorithm presented in class by simulating
// a simplified version of the CPU scheduler


#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <algorithm>
using namespace std;
#include "linuxScheduler.h"

int main()
{
	ofstream out("project3OutputFile.txt");		//redirect cout, prints out to "project3OutputFile.txt"
	auto coutbuf = cout.rdbuf(out.rdbuf());		

	linuxScheduler cpuScheduler;				//Declare linuxScheduler ob
	int clock = 0;								//Clock
	int amtOfProcessesToMove =0;				//Used when mutliple processes need to move from one queue to another at a time (i.e. StartUp -> Active, etc.)
	cpuScheduler.readInput();					//Reads input and stores processes into StartUp Queue

	while (1)			
	{
		amtOfProcessesToMove= 0;
		if (!cpuScheduler.isStartUpEmpty() && cpuScheduler.readyToEnterActive(cpuScheduler.getStartUpQueue(), clock, amtOfProcessesToMove))
			cpuScheduler.moveToActive(cpuScheduler.getStartUpQueue(),amtOfProcessesToMove, clock);	//Move processes from StartUpQueue to ActiveQueue
		if (cpuScheduler.isCPUEmpty() && !cpuScheduler.isActiveEmpty())
			cpuScheduler.moveToCPU(clock);				//move process to CPU
		if (cpuScheduler.isPreempt())
			cpuScheduler.preemptProcess(clock);			//preempts process
		if (!cpuScheduler.isCPUEmpty())
			cpuScheduler.performCPU();					//Performs CPU, and decrements time slice
		if (!cpuScheduler.isIOEmpty())
			cpuScheduler.performIO();					//Performs IO for all processes in IO queue
		if (!cpuScheduler.isCPUEmpty())
		{
			if (cpuScheduler.getCurrentlyRunningProcess().getCurrentCpuTime() == 0)	//If the current CPU Burst Time == 0
			{										
				if (cpuScheduler.isDoneWithAllCpuBursts()){				//If all CPU bursts are done, move process to finish queue
					cpuScheduler.moveToFinished(clock);
				}
				else
					cpuScheduler.moveToIO(clock);						//Else, move to IO queue
			}
			if (cpuScheduler.getCurrentlyRunningProcess().getTimeSlice() == 0){		//If time slice == 0
				cpuScheduler.moveToExpired(cpuScheduler.getCpuQueue(), clock);		//	Move process to Active Queue
			}	
		}
		amtOfProcessesToMove = 0;															
		if (cpuScheduler.readyToFinishIO(amtOfProcessesToMove))					//Check if any processes are done with IO
		{			
			while (amtOfProcessesToMove > 0)
			{					
				if (cpuScheduler.getIoQueue().back().getTimeSlice() == 0)					//If timeslice of process in IO queue == 0
					cpuScheduler.moveToExpired(cpuScheduler.getIoQueue(), clock);			//		Move process to expired
				else
 					cpuScheduler.moveToActive(cpuScheduler.getIoQueue(), 1, clock);			//Else, move process to Active Queue
				amtOfProcessesToMove--;
			}
		}
		if (cpuScheduler.isStartUpEmpty() && cpuScheduler.isActiveEmpty() && cpuScheduler.isIOEmpty() && cpuScheduler.isExpiredEmpty() && cpuScheduler.isCPUEmpty()){
			break;											//If all queues, excpeted finisihed queue, are empty, terminate
		}
		if (cpuScheduler.isActiveEmpty() && cpuScheduler.isCPUEmpty() && !cpuScheduler.isExpiredEmpty()){			
			cpuScheduler.switchActiveAndExpired(clock);		//If Active & CPU are empty, then switch Active and Expired pointers
		}
		++clock;											//Increment clock
	}	
	cpuScheduler.calculateOverallPerformance();				//Calculates and Prints Overall Performance
    cout.rdbuf(coutbuf);									//Restor cout
	return 0;
}











