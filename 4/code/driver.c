/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;
    int cnt = 0;

    in = fopen(argv[1],"r");
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);
        cnt++;

        free(temp);
    }

    fclose(in);

    Time *time = (Time*)malloc(sizeof(Time));
    // invoke the scheduler
    time = schedule();

    printf("Average turnaround time: %f\n", time->average_turnaround_time / cnt);
    printf("Average waiting time: %f\n", time->average_waiting_time / cnt);
    printf("Average response time: %f\n", time->average_response_time / cnt);

    return 0;
}
