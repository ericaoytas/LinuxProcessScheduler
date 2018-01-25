#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
using namespace std;

#include "myProcess.h"

/********* Constructor and Deconstructor **********/

myProcess::myProcess(){
	pid = 0;				//Process ID
	arrivalTime = 0;		//Time that process arrives in Ready Queue
	endTime = 0;			//Time that process terminates/moves to Finished Queue
	priority = 0;			//Priority of Process (based on formula) -- Dynamic Priority
	cpuBursts = NULL;		//Array for CPU Bursts of a Process
	cpuAmt = 0;				//The amount of CPU Bursts
	ioBursts = NULL;		//Array for IO Bursts of a Process
	niceValue = 0;			//Nice value 
	timeSlice = 0;			//Time slice (based on formula)
	cpuIdx = 0;				//Index for CPU Burst Array
	ioIdx = 0;				//Index for IO Bursts Array
	bonus = 0;				//Bonus Value (Based on formula)
	totalCpuUsed = 0;		//Total CPU Used during entire program
	totalIoUsed = 0;		//Total IO Used during entire program
	originalPriority = 0;	//Priority set when in input is read -- Static Priority
}

myProcess::~myProcess(){}

/******************* Setting Functions ********************/

void myProcess::setPid(int processID)
{
	pid = processID;
}
void myProcess::setArrivalTime()
{
	string inputLine;
	getline(cin, inputLine, ' ');
	arrivalTime = stoi(inputLine);
}
void myProcess::setEndTime(int endT)
{
	endTime = endT;
}
bool myProcess::setNiceValue()
{	
	string inputLine;
	getline(cin, inputLine, ' ');
	if (inputLine != "***")
	{
		niceValue = stoi(inputLine);
		return true;
	}
	else
		return false;
}
void myProcess::setCpuAmt()
{
	string inputLine;
	getline(cin, inputLine, ' ');
	cpuAmt = stoi(inputLine);

}
void myProcess::setCpuAndIoTimes()
{
	string inputLine;
	int cpuIndex = 0, ioIndex = 0;
	cpuBursts = new int[cpuAmt];
	ioBursts = new int[cpuAmt-1];
	for (int i = 0; i < (cpuAmt*2)-2; i++)
	{
		getline(cin, inputLine, ' ');
		if (i%2 == 0)
			cpuBursts[cpuIndex++] = stoi(inputLine);
		else 
			ioBursts[ioIndex++] = stoi(inputLine);
	}
	getline(cin, inputLine);
	cpuBursts[cpuIndex++] = stoi(inputLine);
}
void myProcess::setTimeSlice()
{
	timeSlice = (int((1-(priority/150.0))*395+.5) + 5);
}
void myProcess::setInitialPriority()
{
	originalPriority = (int(((niceValue+20)/39.0)*30+.5) + 105);
	priority = originalPriority;
}
void myProcess::setNewPriority()
{
	setBonus();
	priority = originalPriority + bonus;
}
void myProcess::setBonus()
{
	if (totalCpuUsed < totalIoUsed)
		bonus = (int(((1-(totalCpuUsed/double(totalIoUsed)))*-5)-0.5));
	else
		bonus = (int(((1-(totalIoUsed/double(totalCpuUsed)))*5)+0.5));

}
bool myProcess::setInitialProcessInfo(int processID)
{
	if (setNiceValue())
	{
		setArrivalTime();
		setCpuAmt();
		setCpuAndIoTimes();
		setInitialPriority();
		setTimeSlice();
		setPid(processID);
		return false;
	}
	else
		return true;
}

/****************** Getting/Return Value Functions **************/

int myProcess::getPid()
{
	return pid;
}
int myProcess::getArrivalTime()
{
	return arrivalTime;
}
int myProcess::getEndTime()
{
	return endTime;
}
int myProcess::getNiceValue()
{
	return niceValue;
}
int myProcess::getCpuAmt()
{
	return cpuAmt;
}
int myProcess::getPriority()
{
	return priority;
}
int myProcess::getTimeSlice()
{
	return timeSlice;
}
int myProcess::getCurrentCpuTime()
{
	return cpuBursts[cpuIdx];
}
int myProcess::getCurrentIoTime()
{
	return ioBursts[ioIdx];
}
int myProcess::getCpuIdx()
{
	return cpuIdx;
}
int myProcess::getIoIdx()
{
	return ioIdx;
}
int myProcess::getTotalCpuUsed()
{
	return totalCpuUsed;
}
int myProcess::getTotalIoUsed()
{
	return totalIoUsed;
}

/*************** Decrement Functions *************/

void myProcess::decrementTimeSlice()
{
	timeSlice--;
}
void myProcess::decrementCpuTime()
{
	if (cpuBursts[cpuIdx] == 0)				// try switching the if else statements for both cpu and io
		cpuIdx++;

		totalCpuUsed++;
	
		cpuBursts[cpuIdx]--;

}
void myProcess::decrementIoTime()
{
	if (ioBursts[ioIdx] == 0)
		ioIdx++;

		totalIoUsed++;
	
	
	ioBursts[ioIdx]--;
}

/********* Print Function ***********/

void myProcess::printProcessInfo()
{

	cout << "pid: " << getPid() << endl;
	cout << "Nice Value: " << getNiceValue() << endl;
	cout << "Arrival Time: " << getArrivalTime() << endl;
	cout << "Number of CPU Bursts: " << getCpuAmt() << endl << "CPU Burst Times: ";
	for (int j = 0; j < getCpuAmt(); j++)
		cout << cpuBursts[j] << " ";
	cout << endl << "IO Bursts: ";
	for (int j = 0; j < (getCpuAmt()-1); j++)
		cout << ioBursts[j] << " ";
	cout << "\nInitial Priority: " << getPriority() << endl;
	cout << "Time Slice: " << getTimeSlice() << endl;
	cout << endl << endl;


}
