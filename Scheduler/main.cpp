#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <stack>
#include <algorithm>

#include "process.h"
#include "scheduler.h"
#include "event.h"

using namespace std;

#define CR_RE 1
#define RE_RU 2
#define RU_BL 3
#define BL_RE 4
#define RU_RE 5
#define FIN 6

#define SCH_FCFS 1
#define SCH_LCFS 2
#define SCH_SJF 3
#define SCH_RR 4
#define SCH_PR 5

#define LOWESTDP -1

std::vector<int> randomValues;
int totalRandomValues = 0;
int ofs = 0;
bool verbose = false;
Scheduler *sch;
std::vector<Process> processes;
std::vector<Event> events;
int totalTime = 0;
int quantum = -1;
int newProcTime = 0;
int current_time = 0;
double cpu_util = 0.0;
double io_util = 0.0;
double ta_time = 0.0;
double cpuw_time = 0.0;

class Interval {
    public:
        int start;
        int end;
        Interval(int,int);
};

Interval::Interval(int s, int e)
{
    start = s;
    end= e;
}
std::vector<Interval>  ioVector;

//Maintain data structure for IO intervals
void insert_interval(Interval ioInterval)
{
    int ioSize = ioVector.size();
    int pos = ioSize;
    for (int i = 0; i < ioSize; ++i) {
        if (ioInterval.start < ioVector[i].start) {
            pos = i;
            break;
        }
    }
    ioVector.insert(ioVector.begin() + pos, ioInterval);

}

//Comparator method for sorting the intervals
bool compareInterval(Interval i1, Interval i2)
{
    //Sort interval by their start time
    return (i1.start < i2.start)? true: false;
}

//Merge intervals as there will most likely be an overlap in the IO intervals
//Below code is referenced from http://www.geeksforgeeks.org/merging-intervals/
int mergeIntervals()
{   int x = 0;

    if (ioVector.size() <= 0)
        return 0;

    stack<Interval> s;

    sort(ioVector.begin(), ioVector.end(), compareInterval);

    // push the first interval to stack
    s.push(ioVector[0]);

    for (int i = 1 ; i < ioVector.size(); i++)
    {
        Interval top = s.top();

        if (top.end < ioVector[i].start)
        {
            s.push( ioVector[i] );
        }

        else if (top.end < ioVector[i].end)
        {
            top.end = ioVector[i].end;
            s.pop();
            s.push(top);
        }
    }


    while (!s.empty())
    {
        Interval t = s.top();
        x = x + t.end - t.start;
        s.pop();
    }

    return x;
}

//Read processes from the file and add them in the process queue.
void getProcesses(char* fileName)
{
    ifstream fin(fileName);
    int id = 0;

    string word;
    fin>>word;

    while(!fin.eof())
    {

        int arrTime = atoi(word.c_str());

        fin>>word;
        int tCPU = atoi(word.c_str());

        fin>>word;
        int bCPU = atoi(word.c_str());

        fin>>word;
        int bIO = atoi(word.c_str());

        int pID = id++;
        Process proc(pID, arrTime, tCPU, bCPU, bIO);

        processes.push_back(proc);

        fin>>word;

        continue;

    }
    fin.close();
}

//Add events to the event queue
void addEvent(Event e)
{
    //Add events sorted by the time stamp
    int eventSize = events.size();
    int pos = eventSize;
    for (int i = 0; i < events.size(); ++i)
    {
        if (e.timestamp < events[i].timestamp)
        {
            pos = i;
            break;
        }
    }
    events.insert(events.begin() + pos, e);
}


//Read and store the ransom variables
void readRandomNumbers(char *rFile)
{
    ifstream fin(rFile);
    string word;

    fin>>word;
    fin>>word;
    while(!fin.eof())
    {
        int num = atoi(word.c_str());
        randomValues.insert(randomValues.begin()+ofs, num);
        ofs++;
        fin>>word;
    }
    fin.close();
    ofs = 0;
}

//myRandom implementation as in PDF
int getRandomNumber(int burst)
{
    if(ofs == randomValues.size()-1)
        ofs = 0;
    int returnValue = 1 + randomValues[ofs] % burst;
    ofs++;
    return  returnValue;
}

//Get the type of the scheduler
int getSchedulerType(char *argv[], int index)
{
    int schedulerType = -1;
    if(strcmp(argv[index] , "-sF") == 0)
    {
        schedulerType = SCH_FCFS;
    }
    else if(strcmp(argv[index] , "-sL") == 0)
    {
        schedulerType = SCH_LCFS;
    }
    else if(strcmp(argv[index] , "-sS") == 0)
    {
        schedulerType = SCH_SJF;
    }
    else
    {
        if(strchr(argv[index],'R') != NULL)
        {
            schedulerType = SCH_RR;
        }
        else if (strchr(argv[index],'P') != NULL){
            schedulerType = SCH_PR;
        }
    }

    return schedulerType;
}

//Parse the command line data
void setRunningEnvironment(char *argv[]) {
    int schedulerType = -1;
    int index = -1;
    if (strcmp(argv[1], "-v") == 0) {
        index = 2;
        verbose = true;
    }
    else{
        index = 1;
    }
    schedulerType = getSchedulerType(argv, index);
    if(schedulerType == SCH_RR || schedulerType == SCH_PR)
    {
        string q = argv[index] + 3;
        quantum = atoi(q.c_str());
    }

    switch (schedulerType) {
        case SCH_FCFS: sch = new FCFS(SCH_FCFS);break;
        case SCH_LCFS: sch = new LCFS(SCH_LCFS);break;
        case SCH_SJF: sch = new SJF(SCH_SJF);break;
        case SCH_RR: sch = new RoundRobin(SCH_RR,quantum);break;
        case SCH_PR: sch = new Priority(SCH_PR,quantum);break;
        default: sch = NULL;
    }

    char *fileName = argv[index+1];

    getProcesses(fileName);

    readRandomNumbers(argv[index+2]);
}

//Get the first event from the events queue
Event getEvent() {
    Event e;
    if (!events.empty()) {
        e = events.front();
        events.erase(events.begin());
    }
    return e;
}

//Add all new processes to the event queue in order of the process creation;called just once to add the created processes.
void addToEvent()
{
    for(int i = 0; i<processes.size(); i++)
    {
        Event evt(processes[i].A_T, processes[i].P_ID,processes[i].A_T, CR_RE);
        addEvent(evt);
        processes[i].S_PRIO = getRandomNumber(4);
        processes[i].D_PRIO = processes[i].S_PRIO-1;

    }
}

//Method to handle the transition from created state to ready state.
void handleCreatedToReady(int procID, int current_time)
{
    //Add this process to the scheduler's ready queue
    //Also set the previous time of the process to the current time.
    processes[procID].P_T = current_time;
    sch->addProcess(processes[procID]);
    Event e(processes[procID].A_T,procID, processes[procID].A_T, RE_RU);
    addEvent(e);
}

//Method to handle the transition from ready state to running state.
void handleReadyToRunning(Event current_event, int procID)
{

            Process p = sch->getProcess();

            procID = p.P_ID;
            current_event.eventCreationTime = processes[procID].P_T;
            processes[procID].C_W = (processes[procID].C_W + current_time - current_event.eventCreationTime);//Calculate the total CPU time

            if(sch->s_t == SCH_RR||sch->s_t== SCH_PR)
            {
                if(processes[procID].Q_RAN_CB == 0)
                    processes[procID].Q_RAN_CB = getRandomNumber(processes[procID].C_B);

                if(quantum > processes[procID].Q_RAN_CB)
                {

                    if(processes[procID].Q_RAN_CB >= processes[procID].REM_CB)
                    {

                        Event e(current_time+processes[procID].REM_CB, procID, current_time, FIN);
                        newProcTime = current_time + processes[procID].REM_CB;
                        processes[procID].REM_CB  = 0;
                        processes[procID].Q_RAN_CB = 0;
                        addEvent(e);
                    }
                    else
                    {
                        Event e(current_time+processes[procID].Q_RAN_CB, procID, current_time, RU_BL);
                        newProcTime = current_time + processes[procID].Q_RAN_CB;
                        processes[procID].REM_CB -= processes[procID].Q_RAN_CB;
                        processes[procID].Q_RAN_CB = 0;
                        addEvent(e);
                    }

                }
                else if(quantum <= processes[procID].Q_RAN_CB)
                {
                    if(quantum >= processes[procID].REM_CB)
                    {


                        Event e(current_time+processes[procID].REM_CB, procID, current_time, FIN);//Finished state
                        newProcTime = current_time + processes[procID].REM_CB;
                        processes[procID].REM_CB =0 ;
                        processes[procID].Q_RAN_CB = 0;
                        addEvent(e);

                    }
                    else
                    {
                        if(processes[procID].Q_RAN_CB != quantum)
                        {
                            Event e(current_time+quantum, procID, current_time,  RU_RE);
                            newProcTime = current_time + quantum;
                            processes[procID].REM_CB -= quantum;
                            processes[procID].Q_RAN_CB -= quantum;
                            addEvent(e);
                        }
                        else
                        {
                            Event e(current_time+processes[procID].Q_RAN_CB, procID,current_time,RU_BL);
                            newProcTime = current_time + processes[procID].Q_RAN_CB;
                            processes[procID].REM_CB -= processes[procID].Q_RAN_CB;
                            processes[procID].Q_RAN_CB = 0;
                            addEvent(e);
                        }
                    }
                }

            }

            else
            {
                int ran_cb = getRandomNumber(processes[procID].C_B);

                processes[procID].RAN_CB = ran_cb;

                if(processes[procID].REM_CB<=ran_cb)
                {
                    int remBurst = processes[procID].REM_CB;
                    processes[procID].REM_CB = 0; //Set 0 to mark finish state
                    Event e(current_time + remBurst, procID, current_time, FIN);
                    addEvent(e);
                    newProcTime = current_time + remBurst;
                }
                else
                {
                    processes[procID].REM_CB-=ran_cb; //Reduce remaining burst time
                    Event e(current_time + ran_cb, procID, current_time, RU_BL);
                    addEvent(e);
                    newProcTime = current_time + ran_cb;

                }
            }
}

//Method to handle the transition from running state to blocked state.
void handleRunningToBlocked(int procID)
{
    int ran_ib =  getRandomNumber(processes[procID].I_B);

    Event e(current_time + ran_ib, procID, current_time, BL_RE);
    processes[procID].I_T+=ran_ib;
    addEvent(e);
    Interval io(current_time, current_time+ran_ib);
    insert_interval(io);
}

//Method to handle the transition from blocked state to ready state.
void handleBlockedToReady(int procID)
{
    processes[procID].D_PRIO = processes[procID].S_PRIO-1;
    processes[procID].P_T = current_time;
    sch->addProcess(processes[procID]);
    Event e(current_time, procID,current_time,RE_RU);
    addEvent(e);
}

//Method to handle the transition from running state to ready state.
void handleRunningToReady(int procID)
{
    processes[procID].P_T = current_time;
    Event e(current_time, procID,current_time, RE_RU);
    if(sch->s_t==SCH_PR)
    {
    	processes[procID].D_PRIO--;
        if(processes[procID].D_PRIO== LOWESTDP)
        {
            processes[procID].D_PRIO = processes[procID].S_PRIO -1;
            sch->addExpiredProcess(processes[procID]);//Add to the respective expired process
            addEvent(e);
         }
         else
         {
          	addEvent(e);
            sch->addProcess(processes[procID]);
         }
    }
    else
    {
    	addEvent(e);
         sch->addProcess(processes[procID]);
    }
}

//Method to handle finished events
void handleFinishedEvent(int procID)
{
    processes[procID].F_T = current_time;
    processes[procID].T_T = processes[procID].F_T - processes[procID].A_T;
}

//Scheduling starts here
void startExecution()
{
    current_time = processes[0].A_T; //Set arrival of first process as current time
    Event current_event(0,0,0,0); //Dummy event

    while(!events.empty())
    {
        //cout<<"Found event"<<endl;
        current_event = getEvent();

        //printEvent(current_event);

        if(current_time < current_event.timestamp)
            current_time = current_event.timestamp;

        int procID = current_event.procID;

        if(current_event.transitionNumber == CR_RE){
            handleCreatedToReady(procID,current_time);
        }
        else if (current_event.transitionNumber == RE_RU) {

            if (current_event.timestamp < newProcTime) {
                current_event.timestamp = newProcTime;
                addEvent(current_event);
                continue;
            }
            handleReadyToRunning(current_event, procID);
        }
        else if(current_event.transitionNumber == RU_BL){
            handleRunningToBlocked(procID);
        }

        else if(current_event.transitionNumber == BL_RE)
        {
            handleBlockedToReady(procID);

        }
        else if(current_event.transitionNumber == RU_RE)
        {
            handleRunningToReady(procID);
        }
        else if(current_event.transitionNumber == FIN)
        {
            handleFinishedEvent(procID);
        }
    }
}

//Print results as shown in PDF
void printResults()
{
    //Calculate the final IO utilization by merging the IO intervals.
    io_util = mergeIntervals();

    std::string schName = sch->getName();

    cout<<schName;

    if (sch->s_t == SCH_RR || sch->s_t==SCH_PR)
       std::cout << " " << quantum;

    cout<<'\n';

    int processesSize = processes.size();

    for (int i = 0; i < processesSize ; ++i)
    {
        int procID = processes[i].P_ID;
        int arr_time = processes[i].A_T;
        int tot_CPU_time = processes[i].T_C;
        int cpu_burst = processes[i].C_B;
        int io_burst = processes[i].I_B;
        int static_prio = processes[i].S_PRIO;
        int fin_time = processes[i].F_T;
        int turnAround = processes[i].T_T;
        int io_time  = processes[i].I_T;
        int cpu_waitTime = processes[i].C_W;

        printf("%04d: %4d %4d %4d %4d %d |  %4d  %4d  %4d  %4d\n", procID, arr_time, tot_CPU_time, cpu_burst, io_burst, static_prio,  fin_time, turnAround , io_time, cpu_waitTime);
        if (processes[i].F_T > totalTime)
           totalTime = processes[i].F_T;

	cpuw_time += cpu_waitTime;
	ta_time += turnAround;
        	cpu_util += tot_CPU_time;
    }
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", totalTime, (cpu_util*100)/totalTime, (io_util*100)/totalTime, ta_time/processesSize, cpuw_time/processesSize, ((double)processesSize*100)/totalTime);
}
int main(int argc, char *argv[])
{
    if (argc < 3) {
        cout<<"Invalid input parameters";
        exit(0);
    }

 //   cout<<"Setting running environment"<<endl;
    setRunningEnvironment(argv);

 //   cout<<"Adding newly created processes to event"<<endl;
    addToEvent();

  //  cout<<"Starting scheduling"<<endl;
    startExecution();

  //  cout<<"Printing results"<<endl;
    printResults();

    return 0;
}
