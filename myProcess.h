#include <iostream>
#include <string>

using namespace std;

#ifndef myProcess_h
#define myProcess_h

class myProcess
{
private:
	int pid;
	int arrivalTime;
	int endTime;
	int *cpuBursts;
	int cpuAmt;
	int *ioBursts;
	int niceValue;
	int priority;
	int originalPriority;
	int timeSlice;
	int cpuIdx;
	int ioIdx;
	int bonus;
	int totalCpuUsed;
	int totalIoUsed;

public:
	myProcess();
	~myProcess();

	//setting functions
	void setPid(int);
	void setArrivalTime();
	void setEndTime(int);
	bool setNiceValue();
	void setCpuAmt();
	void setCpuAndIoTimes();
	void setInitialPriority();
	void setNewPriority();
	void setTimeSlice();
	void setBonus();
	bool setInitialProcessInfo(int);

	//getting/return functions
	int getPid();
	int getArrivalTime();
	int getEndTime();
	int getNiceValue();
	int getCpuAmt();
	int getPriority();
	int getTimeSlice();
	int getCurrentCpuTime();
	int getCurrentIoTime();
	int getCpuIdx();
	int getIoIdx();
	int getTotalCpuUsed();
	int getTotalIoUsed();

	//decrement functions
	void decrementTimeSlice();
	void decrementCpuTime();
	void decrementIoTime();

	//print functions
	void printProcessInfo();
};

#endif