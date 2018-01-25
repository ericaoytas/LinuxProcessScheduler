#include <iostream>
#include <string>
#include<vector>
#include "myProcess.h"
using namespace std;

#ifndef linuxScheduler_h
#define linuxScheduler_h

enum sortType{BY_ARRIVAL, BY_PRIORITY, BY_ENDTIME, BY_IOTIME};

class linuxScheduler: public myProcess
{
private:
	vector<myProcess> startUpQueue;
	vector<myProcess> activeQueue;
	vector<myProcess> expiredQueue;
	vector<myProcess> ioQueue;
	vector<myProcess> finishedQueue;
	vector<myProcess> CPU;
public:
	linuxScheduler();
	~linuxScheduler();
	//set functions
	void readInput();
	void setStartUpQueue(vector<myProcess>);
	void setActiveQueue(myProcess);
	void setExpiredQueue(myProcess);
	void setRunningProcess(myProcess);
	void setIoQueue(myProcess);
	void setFinishedQueue(myProcess);

	//get functions
	vector<myProcess> getStartUpQueue();
	vector<myProcess> getActiveQueue();
	vector<myProcess> getIoQueue();
	vector<myProcess> getCpuQueue();
	vector<myProcess> getFinishedQueue();
	myProcess getCurrentlyRunningProcess();

	//bool functions
	bool isStartUpEmpty();
	bool isActiveEmpty();
	bool isExpiredEmpty();
	bool isIOEmpty();
	bool isCPUEmpty();
	bool isFinishedEmpty();
	bool readyToEnterActive(vector<myProcess>, int, int&);
	bool readyToFinishIO(int&);
	bool isPreempt();
	bool isDoneWithAllCpuBursts();

	//moving functons
	void moveToActive(vector<myProcess>,int, int);
	void moveToExpired(vector<myProcess>, int);
	void moveToCPU(int);
	void moveToIO(int);
	void moveToFinished(int);
	void preemptProcess(int);
	void switchActiveAndExpired(int);

	//cpu/io perform functions
	void performCPU();
	void performIO();

	//sorting & compare functions
	void sortVector(vector<myProcess>&, sortType);
	static bool comparePriorities(myProcess, myProcess);
	static bool compareArrivalTimes(myProcess, myProcess);
	static bool compareEndTimes(myProcess, myProcess);
	static bool compareIoBurstTimes(myProcess, myProcess);

	// print functions
	void calculateOverallPerformance();
	void printInfo(vector<myProcess>);	//used for testing
};


#endif 