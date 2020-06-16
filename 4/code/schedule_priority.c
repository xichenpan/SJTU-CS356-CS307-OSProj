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

    struct node *tmp, *head;

    while(tasks != NULL){
        tmp = head = tasks;

        while(tmp != NULL) {  
            if (tmp->task->priority > head->task->priority) {
                head = tmp;
            }
            tmp=tmp->next;                                            
        }

        head->task->start = cu_time;
        time->average_waiting_time += cu_time;
        time->average_response_time += cu_time;
        run(head->task, head->task->burst);

        cu_time += head->task->burst;
        time->average_turnaround_time += cu_time;
        delete(&tasks, head->task);
    }
    return time;
}
