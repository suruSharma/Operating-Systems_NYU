#include "event.h"

Event::Event(int t_s,int p_id, int c_t, int t_n)
{
    timestamp = t_s;
    procID = p_id;
    eventCreationTime = c_t;
    transitionNumber = t_n;
}
Event::Event()
{

}
