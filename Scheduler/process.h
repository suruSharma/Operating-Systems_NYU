#ifndef PROCESS_H
#define PROCESS_H


class Process
{
    public:

        int P_ID;//process ID
        int A_T; //arrival time
        int T_C; //total CPU time
        int C_B; //CPU burst time
        int I_B; //IO Burst time
        int P_T;
        int F_T;
        int T_T;
        int C_W;
        int I_T;
        int REM_CB;
        int RAN_CB;
        int S_PRIO;
        int D_PRIO;
        int Q_RAN_CB;
        Process(int, int, int, int, int);
        Process();

    protected:
    private:
};

#endif // PROCESS_H
