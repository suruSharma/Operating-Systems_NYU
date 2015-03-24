#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "process.h"
using namespace std;
class Scheduler
{
    public:
        vector <Process> readyQueue;
        int s_t;
        int q; //quantum required for RR and Priority
        virtual Process getProcess() = 0;
        virtual void addProcess(Process) = 0;
        virtual std::string getName() = 0;

        vector<Process> active_0;
        vector<Process> active_1;
        vector<Process> active_2;
        vector<Process> active_3;

        vector<Process> expired_0;
        vector<Process> expired_1;
        vector<Process> expired_2;
        vector<Process> expired_3;

        virtual void addExpiredProcess(Process) = 0;

    protected:
    private:

};
class FCFS : public Scheduler {
    public:
        FCFS(int);
        void addProcess(Process);
        Process getProcess();
        std::string getName();
        void addExpiredProcess(Process);
};

class LCFS : public Scheduler {
    public:
        LCFS(int);
        void addProcess(Process);
        Process getProcess();
        std::string getName();
        void addExpiredProcess(Process);
};

class SJF : public Scheduler {
    public:
        SJF(int);
        void addProcess(Process);
        Process getProcess();
        std::string getName();
        void addExpiredProcess(Process);
};

class RoundRobin : public Scheduler {
    private:
    public:
        RoundRobin(int, int);
        void addProcess(Process);
        Process getProcess();
        std::string getName();
        void addExpiredProcess(Process);
};

class Priority : public Scheduler {
    private:
    public:
        Priority(int, int);
        void addProcess(Process);
        Process getProcess();
        std::string getName();
        void addExpiredProcess(Process);
};

#endif // SCHEDULER_H
