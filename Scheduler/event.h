#ifndef EVENT_H
#define EVENT_H


class Event
{
    public:
        Event(int,int,int,int);
        Event();
        int timestamp;
        int eventCreationTime;
        int procID;
        int transitionNumber;
    protected:
    private:
};

#endif // EVENT_H
