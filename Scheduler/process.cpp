#include "process.h"

Process :: Process()
{
    P_ID = A_T = T_C = C_B = I_B =RAN_CB= F_T = T_T =REM_CB= I_T = C_W = P_T=S_PRIO=D_PRIO=Q_RAN_CB=0;
}

Process :: Process(int p_id, int a_t, int t_c, int c_b, int i_b)
{
    P_ID = p_id;
    A_T = a_t;
    REM_CB = t_c;
    T_C = t_c;
    C_B = c_b;
    I_B = i_b;
    F_T = 0;
    T_T = 0;
    I_T = 0;
    C_W = 0;
    P_T = 0;
    RAN_CB = 0;
    D_PRIO = 0;
    S_PRIO = 0;
    Q_RAN_CB = 0;

}




