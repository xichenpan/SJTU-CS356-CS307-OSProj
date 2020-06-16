#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

typedef struct Time
{
    float average_turnaround_time;
    float average_waiting_time;
    float average_response_time;
}Time;

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
Time* schedule();
