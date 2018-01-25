#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <math.h>
using namespace std;

#include "linuxScheduler.h"

/********** Constructor and Deconstructor **********************/

linuxScheduler::linuxScheduler(){}
linuxScheduler::~linuxScheduler(){}

//******* Set (setting private members) functions **************/

void linuxScheduler::readInput()
//Function reads input from file and stores into vector. Then, calls setStartUpQueue, passing the vector, to set up start Up queue
{
	vector<myProcess> processList;
	vector<myProcess>::iterator it = processList.begin();
	int processCount = 0;
	bool endOfFile = false;
	while (!endOfFile)
	{				
		myProcess currentProcess;
		if ((endOfFile = currentProcess.setInitialProcessInfo(processCount++)) == false)
			it = processList.insert(it,currentProcess);
	}
	setStartUpQueue(processList);
}


void linuxScheduler::setStartUpQueue(vector<myProcess> startQ)
//Function sets up the Start Up Queue
//Passed: Vector 
{
	startUpQueue = startQ;
	sortVector(startUpQueue, BY_ARRIVAL);
}

void linuxScheduler::setActiveQueue(myProcess newProcess)
//Function sets up Active Queue, and then sorts by arrival and then by priority
//Passed: Process to put into Active Queue
{
	activeQueue.push_back(newProcess);
	sortVector(activeQueue, BY_ARRIVAL);
	sortVector(activeQueue, BY_PRIORITY);	
}

void linuxScheduler::setExpiredQueue(myProcess currentProcess)
//Function sets up Expired Queue, then sorts queue by priority
//Passed: Process to put into Expired Queue
{
	expiredQueue.push_back(currentProcess);
	sortVector(expiredQueue, BY_PRIORITY);
}

void linuxScheduler::setRunningProcess(myProcess newProcess)
//Function sets up the Currently Running Process in CPU
//Passed: Process 
{
	CPU.push_back(newProcess);
}

void linuxScheduler::setIoQueue(myProcess ioProcess)
//Function sets up IO Queue, then sorts by IO Time
//Passed: Process to put into IO Queue
{
	ioQueue.push_back(ioProcess);
	sortVector(ioQueue, BY_IOTIME);
}

void linuxScheduler::setFinishedQueue(myProcess finishedProcess)
//Function sets up Finished Queue, then sorts by finished times
//Passed: process to put into Finished Queue
{
	finishedQueue.push_back(finishedProcess);
	sortVector(finishedQueue, BY_ENDTIME);
}

/*************** get (return private member) functions *********/

vector<myProcess> linuxScheduler::getStartUpQueue()
//Returns Start Up Queue
{
	return startUpQueue;
}

vector<myProcess> linuxScheduler::getActiveQueue()
//Returns Active Queue
{
	return activeQueue;
}

vector<myProcess> linuxScheduler::getIoQueue()
//Returns IO Queue
{
	return ioQueue;
}

vector<myProcess> linuxScheduler::getCpuQueue()
//Returns CPU Queue
{
	return CPU;
}

vector<myProcess> linuxScheduler::getFinishedQueue()
//Returns Finished Queue
{
	return finishedQueue;
}

myProcess linuxScheduler::getCurrentlyRunningProcess()
//Returns the process in CPU
{
	return CPU.back();
}

/******************** bool functions *****************/

//-------- check if empty functions --------

bool linuxScheduler::isStartUpEmpty()
//Returns TRUE if Start Up Queue is empty, FALSE otherwise
{
	return startUpQueue.empty();
}

bool linuxScheduler::isActiveEmpty()
// Returns TRUE if Active Queue is empty, FALSE otherwise
{
	return activeQueue.empty();
}

bool linuxScheduler::isExpiredEmpty()
// Returns TRUE if Expired Queue is empty, FALSE otherwise
{
	return expiredQueue.empty();
}

bool linuxScheduler::isIOEmpty()
//Returns TRUE if IO Queue is empty, FALSE otherwise
{
	return ioQueue.empty();
}	
bool linuxScheduler::isCPUEmpty()
//Returns TRUE if CPU is empty, FALSE otherwise
{
	return CPU.empty();
}
bool linuxScheduler::isFinishedEmpty()
//Returns TRUE if Finished Queue is empty, FALSE otherwise
{
	return finishedQueue.empty();
}


//--------- other bools, to check if ready to move to other queues ---------

bool linuxScheduler::readyToEnterActive(vector<myProcess> currentQueue, int clock, int &amtOfReadyProcess)
//Function checks if process is ready to enter Active Queue from the Start Up Queue, and passes by reference the amount of processes ready to move
//Returns TRUE if there is at least one process that has arrived and is ready to move, FALSE otherwise
//Passed: Vecter Queue, Clock, amount of ready processes(by reference)
{
	myProcess p1 = currentQueue.back();
	int arrivalTimeToCheck = p1.getArrivalTime();
	if (arrivalTimeToCheck == clock)
	{
		for (vector<myProcess>::iterator it = currentQueue.end()-1; it >= currentQueue.begin(); --it)
		{

			if (it->getArrivalTime() == arrivalTimeToCheck)
				amtOfReadyProcess++;
			else
				break;
		}
		return true;
	}
	else
		return false;
}

bool linuxScheduler::readyToFinishIO(int &amtOfProcessesToMove)
// Function checks if any processes in the IO queue are finished doing IO and are ready to move, passes by reference the amount of processes ready
// Returns TRUE if there is at least one IO done, FALSE otherwise
// Passed: amount of processes to move (by reference)
{
	vector<myProcess> processList = getIoQueue();
	if (!isIOEmpty())
	{
		for (vector<myProcess>::iterator it = processList.end()-1; it >= processList.begin(); --it)
		{
			if (it->getCurrentIoTime() == 0)
				amtOfProcessesToMove++;
			else
				break;
		}
		return true;
	}
	else
		return false;
}

bool linuxScheduler::isPreempt()
// Function checks if the process in CPU is to be preempted, depending on priorities
// Return TRUE if the new process will preempt the running process, FALSE otherwise
{
	if (!isCPUEmpty() && !isActiveEmpty())
		if (activeQueue.back().getPriority() < CPU.back().getPriority())
			return true;
	else
		return false;
}

bool linuxScheduler::isDoneWithAllCpuBursts()
// Function checks if the process in CPU is done with all CPU bursts (i.e. done with everything)
// Returns TRUE if running process is done with all CPU Bursts, FALSE otherwise
{
	myProcess runningProcess = CPU.back();
	if (runningProcess.getCpuIdx() == runningProcess.getCpuAmt()-1){
		return true;
	}
	else
		return false;
}


/********** Moving Functions (Moving from one state to another) ********/

void linuxScheduler::moveToActive(vector<myProcess> currentQueue, int amtOfProcessesToMove, int clock)
// Function moves process(es) from the passed queue (either start up queue or io queue) and then calls setActiveQueue() to place process(es) in Active Queue
// Passed: vector queue, amount of process to move (by ref), clock
{
	while (amtOfProcessesToMove-- > 0)
	{
		myProcess currentProcess;
		if (!startUpQueue.empty() && currentQueue.back().getPid() == startUpQueue.back().getPid())		//if source queue is startUpQueue
		{
			currentProcess = startUpQueue.back();
			startUpQueue.pop_back();
			cout << "[" << clock << "] " << "<" << currentProcess.getPid() << "> Enters ready queue (Priority: " << currentProcess.getPriority() << ", TimeSlice: " << currentProcess.getTimeSlice() << ")" << endl;
		}
		else																	//else source queue is ioQueue
		{			
			currentProcess = ioQueue.back();
			ioQueue.pop_back();
			cout << "[" << clock << "] " << "<" << currentProcess.getPid() << "> Finishes IO and moves to the Ready Queue" << endl;
		}
		currentQueue.pop_back();
		setActiveQueue(currentProcess);
	}	
}

void linuxScheduler::moveToExpired(vector<myProcess> currentQueue, int clock)
// Function moves process from passed queue (either CPU or Io Queue) and calls setExpiredQueue() to place process into Expired Queue
// Passed: Vector Queue, clock
{
	myProcess currentProcess;
	if (currentQueue.back().getPid() == CPU.back().getPid())							//if source queue is cpuQueue
	{		
		currentProcess = CPU.back();
		CPU.pop_back();	
		currentProcess.setNewPriority();
		currentProcess.setTimeSlice();
		cout << "[" << clock << "] <" << currentProcess.getPid() << "> Finishes its time slice and moves to the Expired Queue (Priority: " << currentProcess.getPriority() << ", Timeslice: " << currentProcess.getTimeSlice() << ")" << endl;
	}
	else																		//else source queue is ioQueue
	{
		currentProcess = ioQueue.back();
		ioQueue.pop_back();
		currentProcess.setNewPriority();
		currentProcess.setTimeSlice();
		cout << "[" << clock << "] <" << currentProcess.getPid() << "> Finishes IO and moves to the Expired Queue (Priority: " << currentProcess.getPriority() << ", Timeslice: " << currentProcess.getTimeSlice() << ")" << endl;
	}
	setExpiredQueue(currentProcess);
}

void linuxScheduler::moveToCPU(int clock)
// Function moves a process from Active Queue, and calls setRunningProcess() to place into CPU
// Passed: Clock (for printing purposes)
{
	myProcess currentProcess = activeQueue.back();
	setRunningProcess(currentProcess); //inserts last element
	activeQueue.pop_back();
	cout << "[" << clock << "] <" << currentProcess.getPid() << "> Enters the CPU" << endl;
}

void linuxScheduler::moveToIO(int clock)
// Function moves a process from CPU, and calls setIoQueue() to place into IO Queue
// Passed: Clock 
{
	myProcess currentProcess = CPU.back();
	setIoQueue(currentProcess); //inserts last element
	CPU.pop_back();
	cout << "[" << clock << "] <" << currentProcess.getPid() << "> Moves to the IO Queue" << endl;
}

void linuxScheduler::moveToFinished(int clock)
// Function moves a process from CPU, and calls setFinishedQueue() to place into Finished Queue. Function also calls setEndTime() to set the finished time of the process
// Passed: Clock
{
	myProcess currentProcess = CPU.back();
	currentProcess.setEndTime(clock);
	setFinishedQueue(currentProcess); //inserts last element
	CPU.pop_back();
	cout << "[" << clock << "] <" << currentProcess.getPid() << "> Finishes and moves to the Finished Queue" << endl;
}

void linuxScheduler::preemptProcess(int clock)
// Function moves process from Active to CPU by calling setRunningProcess(), and moves the running process CPU to Active by calling setActiveQueue()
// Passed: Clock
{
	myProcess newProcess = activeQueue.back();
	myProcess runningProcess = CPU.back();
	activeQueue.pop_back();
	CPU.pop_back();
	setRunningProcess(newProcess);
	setActiveQueue(runningProcess);
	cout << "[" << clock << "] <" << newProcess.getPid() << "> Preempt Process " << runningProcess.getPid() << endl;
}

void linuxScheduler::switchActiveAndExpired(int clock)
// Function switches the active and expired queues
// Passed: Clock
{
	vector<myProcess> tempQueue;
	tempQueue = activeQueue;
	activeQueue = expiredQueue;
	expiredQueue = tempQueue;
	cout << "[" << clock << "] *** Queue Swap" << endl;
}

//**** Performing CPU or IO functions *******

void linuxScheduler::performCPU()
// Function performs CPU, by decrementing time slice (by calling decrementTimeSlice()) and by decrementing CPU Burst Time (by calling decrementCpuTime())
{
	CPU.back().decrementTimeSlice();
	CPU.back().decrementCpuTime();
}

void linuxScheduler::performIO()
// Function performs IO for all process in IO Queue by decrement IO Burst (by calling decrementIoTime())
{
	for (vector<myProcess>::iterator it = ioQueue.begin(); it < ioQueue.end(); ++it)
		it->decrementIoTime();
}

//****** Sorting & Comparative Functions **************

void linuxScheduler::sortVector(vector<myProcess> &processList, sortType sortMethod)
// Function sorts the give queue by the given method. Choices of sort type: Arrival times, priorities, end times, and IO burst times
// Passed: Queue with processes to sort, type of sort
{
	switch (sortMethod)
	{
		case BY_ARRIVAL:
			sort(processList.begin(), processList.end(), compareArrivalTimes);
			break;
		case BY_PRIORITY:
			sort(processList.begin(), processList.end(), comparePriorities);
			break;
		case BY_ENDTIME:
			sort(processList.begin(), processList.end(), compareEndTimes);
			break;
		case BY_IOTIME:
			sort(processList.begin(), processList.end(), compareIoBurstTimes);
			break;
		default:
		break;
	}
}


bool linuxScheduler::compareArrivalTimes(myProcess firstProcess, myProcess secondProcess)
// Function compares arrival times
// Passed: 2 processes
{
	return (firstProcess.getArrivalTime() > secondProcess.getArrivalTime());
}

bool linuxScheduler::comparePriorities(myProcess firstProcess, myProcess secondProcess)
// Function compares priorities
// Passed: 2 processes
{
	return (firstProcess.getPriority() > secondProcess.getPriority());
}

bool linuxScheduler::compareEndTimes(myProcess firstProcess, myProcess secondProcess)
// Function compares end times (time processes were moved to finished queue)
// Passed: 2 processes
{
	return (firstProcess.getEndTime() > secondProcess.getEndTime());
}

bool linuxScheduler::compareIoBurstTimes(myProcess firstProcess, myProcess secondProcess)
// Function compares IO burst times
// Passed: 2 processes
{
	return (firstProcess.getCurrentIoTime() > secondProcess.getCurrentIoTime());
}


//********* Printing Functions *************

void linuxScheduler::printInfo(vector<myProcess> processList)
// Function iterates through given vector and calls printProcessInfo() to print the process information ------ For checking purposes
// Passed: vector queue
{
	for (vector<myProcess>::iterator it = processList.begin(); it < processList.end(); ++it)
		it->printProcessInfo();
}

void linuxScheduler::calculateOverallPerformance()
// Function prints performance for each process, such as Turnaround Time, Total CPU Time, Waiting Time, and Percentage of CPU Utilization
// Then, function prints the averages of the turnaround time, waiting time and CPU utilization
{
	double runningTTsum = 0;
	double runningWTsum = 0;
	float runningCPUsum = 0;
	int amtOfProcesses = finishedQueue.size();
	cout << "OUTPUT:" << endl;
	

	while (!finishedQueue.empty())
	{	
		myProcess currentProcess = finishedQueue.back();	
		int turnaroundTime = currentProcess.getEndTime() - currentProcess.getArrivalTime();
		int totalCpu = currentProcess.getTotalCpuUsed();
		int waitingTime = turnaroundTime - totalCpu - currentProcess.getTotalIoUsed();
		float cpuUtil = (totalCpu/float(turnaroundTime))*100;

		runningCPUsum += cpuUtil;

		cpuUtil = floor((cpuUtil)*10+.5)/10;	

		cout << "PID: <" << currentProcess.getPid() << "> Statistics:" << endl;
		cout << "1. Turnaround Time: " << turnaroundTime << endl;
		cout << "2. Total CPU Time: " << totalCpu << endl;
		cout << "3. Waiting Time: " << waitingTime << endl;
		cout << "4. Percentage of CPU utilization time: " << cpuUtil << endl;
		cout << "-------------------------------------" << endl;

		runningTTsum += turnaroundTime;
		runningWTsum += waitingTime;

		finishedQueue.pop_back();
	}
	cout << "\n ===== OVERALL PERFORMANCE ===== " << endl;

	cout << setprecision(3) << fixed << endl;
	cout << "Average Waiting Time: " << (runningWTsum/amtOfProcesses) << endl;
	cout << "Average Turnaround Time: " << (runningTTsum/amtOfProcesses) << endl;
	cout << "Average CPU Utilization: " << (runningCPUsum/amtOfProcesses) << endl << endl;
}







