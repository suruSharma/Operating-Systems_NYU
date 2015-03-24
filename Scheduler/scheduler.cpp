#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "scheduler.h"

using namespace std;
FCFS::FCFS(int type)
{
    s_t = type;
}

void FCFS::addProcess(Process p)
{
    //Simply add process to ready queue
    readyQueue.push_back(p);
}

Process FCFS::getProcess()
{
    Process proc;
    if(!readyQueue.empty())
    {
        //If ready queue has data, return the first process. FIFO
        proc = readyQueue.front();
        readyQueue.erase(readyQueue.begin());
    }
    return proc;
}

string FCFS::getName()
{
    return "FCFS";
}

void FCFS::addExpiredProcess(Process p)
{
}

LCFS::LCFS(int type)
{
    s_t = type;
}

void LCFS::addProcess(Process p)
{
    //Simply add process to ready queue
    readyQueue.push_back(p);
}

Process LCFS::getProcess()
{
    Process proc;
    if(readyQueue.size()!=0)
    {
        //If ready queue has data, return the last process. LIFO
        int readyQueueSize = readyQueue.size();
        proc = readyQueue[readyQueueSize-1];
        readyQueue.pop_back();
    }
    return proc;
}

string LCFS::getName()
{
    return "LCFS";
}

void LCFS::addExpiredProcess(Process p)
{
}

SJF::SJF(int type)
{
    s_t = type;
}

void SJF::addProcess(Process proc)
{
    //Add processes sorted by their remaining burst time(shortest one first)
    int readyQueue_size = readyQueue.size();
    int proc_pos = readyQueue_size;
    for (int i = 0; i < readyQueue_size; ++i) {
        if (proc.REM_CB < readyQueue[i].REM_CB) {
            proc_pos = i;
            break;
        }
    }
    readyQueue.insert(readyQueue.begin() + proc_pos, proc);
}

Process SJF::getProcess()
{
    Process proc;
    if(readyQueue.size()!=0)
    {
        proc = readyQueue.front();
        readyQueue.erase(readyQueue.begin());
    }
    return proc;
}

string SJF::getName()
{
    return "SJF";
}

void SJF::addExpiredProcess(Process p)
{
}


RoundRobin::RoundRobin(int type, int quantum)
{
    s_t = type;
    q = quantum;
}


void RoundRobin::addProcess(Process proc)
{
    //Simply add process to ready queue
    readyQueue.push_back(proc);
}

Process RoundRobin::getProcess()
{
    Process proc;
    if(readyQueue.size()!=0)
    {
        proc = readyQueue.front();
        readyQueue.erase(readyQueue.begin());
    }
    return proc;
}

string RoundRobin::getName()
{
    return "RR";
}

void RoundRobin::addExpiredProcess(Process p)
{
}


Priority::Priority(int type, int quantum)
{
    s_t = type;
    q = quantum;
}


void Priority::addProcess(Process proc)
{
    //Add processes according to their priority in the active queues
    if(proc.D_PRIO==0)
        active_0.push_back(proc);
    else if(proc.D_PRIO==1)
        active_1.push_back(proc);
    else if(proc.D_PRIO==2)
        active_2.push_back(proc);
    else if(proc.D_PRIO==3)
        active_3.push_back(proc);
}

Process Priority::getProcess()
{
    Process proc;
    if(active_0.empty() && active_1.empty() && active_2.empty() && active_3.empty())
    {
        active_0.swap(expired_0);
        active_1.swap(expired_1);
        active_2.swap(expired_2);
        active_3.swap(expired_3);
    }

    //Return the first process with highest priority
    if(active_3.size()!=0)
    {
        proc = active_3.front();
        active_3.erase(active_3.begin());
    }
    else if(active_2.size()!=0)
    {
        proc = active_2.front();
        active_2.erase(active_2.begin());
    }
    else if(active_1.size()!=0)
    {
        proc = active_1.front();
        active_1.erase(active_1.begin());
    }
    else if(active_0.size()!=0)
    {
        proc = active_0.front();
        active_0.erase(active_0.begin());
    }

    return proc;
}

string Priority::getName()
{
    return "PRIO";
}

void Priority::addExpiredProcess(Process proc)
{
    //Add processes according to their priority in the expired queues
    if(proc.D_PRIO==0)
        expired_0.push_back(proc);
    else if(proc.D_PRIO==1)
        expired_1.push_back(proc);
    else if(proc.D_PRIO==2)
        expired_2.push_back(proc);
    else if(proc.D_PRIO==3)
        expired_3.push_back(proc);
}
