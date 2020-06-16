#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node *tasks = NULL;

void add(char *name, int priority, int burst)
{
    Task *t = malloc(sizeof(Task));

    t->name = name;
    t->priority = priority;
    t->burst = burst;
    t->flag = 0;
    t->start = 0;
    t->end = 0;

    insert(&tasks, t);
}

Time* schedule()
{
    Time *time = (Time*)malloc(sizeof(Time));
    time->average_turnaround_time = 0;
    time->average_waiting_time = 0;
    time->average_response_time = 0;
    int cu_time = 0;

    struct node *cu_task, *pos = tasks;
    int run_time;

    while (tasks != NULL) {
        cu_task = pos;

        if (pos->next != NULL)
            pos = pos->next;
        else pos = tasks;
        
        if (QUANTUM < cu_task->task->burst)
            run_time = QUANTUM;
        else
            run_time = cu_task->task->burst;

        if (cu_task->task->flag == 0)
        {
            cu_task->task->flag = 1;
            cu_task->task->start = cu_time;
            time->average_waiting_time += cu_time;
            time->average_response_time += cu_time;
        }
        else
        {
            time->average_waiting_time += cu_time - cu_task->task->end;
        }
        run(cu_task->task, run_time);
        cu_time += run_time;
        cu_task->task->burst -= run_time;
        cu_task->task->end = cu_time;

        if (cu_task->task->burst == 0) {
            delete(&tasks, cu_task->task);
            time->average_turnaround_time += cu_task->task->end;
        }
    }
    return time;
}
