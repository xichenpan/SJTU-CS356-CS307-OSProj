#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define MAX_HOLE_NUM 50

typedef struct {
    bool use;
    int start;
    int end;
    int size;
    char process[10];
} hole;

bool request(char process[], int request_size, char strategy);

void release(char process[]);

void compact();

void report();

hole memory[MAX_HOLE_NUM];
int length = 0;
char cmd[5];

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int size = atoi(argv[1]);
    char process[10];
    int request_size;
    char strategy;

    memory[0].use = false;
    memory[0].start = 0;
    memory[0].end = size - 1;
    memory[0].size = size;
    length++;

    for (int i = 1; i < MAX_HOLE_NUM; i++)
        memory[i].use = false;

    while (true) {
        printf("allocator> ");
        scanf("%s", cmd);
        if (strcmp(cmd, "X") == 0)
            break;
        else if (strcmp(cmd, "RQ") == 0) {
            scanf(" %s %d %ch", process, &request_size, &strategy);
            request(process, request_size, strategy);
        }
        else if (strcmp(cmd, "RL") == 0) {
            scanf(" %s", process);
            release(process);
        }
        else if (strcmp(cmd, "C") == 0)
            compact();
        else if (strcmp(cmd, "STAT") == 0)
            report();
        else
            printf("error input\n");
    }
    return 0;
}

bool request(char process[], int request_size, char strategy) {
    int index = 0;
    int i;
    bool flag = false;
    if (strategy == 'F') {
        for (i = 0; i < length; i++)
            if (!memory[i].use && memory[i].size >= request_size) {
                flag = true;
                break;
            }
        index = i;
    }
    else if (strategy == 'B') {
        int min = INT_MAX;
        for (i = 0; i < length; i++)
            if (!memory[i].use && memory[i].size >= request_size && memory[i].size < min) {
                flag = true;
                index = i;
                min = memory[i].size;
            }
    }
    else if (strategy == 'W') {
        int max = 0;
        for (i = 0; i < length; i++)
            if (!memory[i].use && memory[i].size >= request_size && memory[i].size > max) {
                flag = true;
                index = i;
                max = memory[i].size;
            }
    }
    else {
        printf("error strategy\n");
        return false;
    }

    if (!flag) {
        printf("no space to allocate\n");
        return false;
    }

    if (memory[index].size == request_size) {
        strcpy(memory[index].process, process);
        memory[index].use = true;
        return true;
    }

    for (i = length - 1; i >= index; i--) {
        memory[i + 1].use = memory[i].use;
        memory[i + 1].start = memory[i].start;
        memory[i + 1].end = memory[i].end;
        memory[i + 1].size = memory[i].size;
        strcpy(memory[i + 1].process, memory[i].process);
    }
    length++;
    memory[index].use = true;
    memory[index].end = memory[index].start + request_size - 1;
    memory[index].size = request_size;
    strcpy(memory[index].process, process);
    memory[index + 1].start = memory[index].end + 1;
    memory[index + 1].size = memory[index + 1].end - memory[index + 1].start + 1;
    return true;
}

void release(char process[]) {
    int index;
    int i;
    for (i = 0; i < length; i++)
        if (memory[i].use && strcmp(process, memory[i].process) == 0)
            break;
    if (i == length) {
        printf("process not found\n");
        return;
    }
    index = i;

    memory[index].use = false;
    if (index < length - 1 && !memory[index + 1].use) {
        memory[index].end = memory[index + 1].end;
        memory[index].size += memory[index + 1].size;
        for (i = index + 1; i < length; i++) {
            memory[i].use = memory[i + 1].use;
            memory[i].start = memory[i + 1].start;
            memory[i].end = memory[i + 1].end;
            memory[i].size = memory[i + 1].size;
            strcpy(memory[i].process, memory[i + 1].process);
        }
        length--;
    }
    if (index >= 1 && !memory[index - 1].use) {
        memory[index - 1].end = memory[index].end;
        memory[index - 1].size += memory[index].size;
        for (i = index; i < length; i++) {
            memory[i].start = memory[i + 1].start;
            memory[i].end = memory[i + 1].end;
            memory[i].size = memory[i + 1].size;
            strcpy(memory[i].process, memory[i + 1].process);
            memory[i].use = memory[i + 1].use;
        }
        length--;
    }
}

void compact() {
    int used_hole_index[MAX_HOLE_NUM];
    int cnt = 0;
    int unused = 0;
    for (int i = 0; i < length; i++) {
        if (memory[i].use) {
            used_hole_index[cnt] = i;
            cnt++;
        }
        else
            unused += memory[i].size;
    }

    if (cnt == 0) return;

    for (int i = 0; i < cnt; i++) {
        memory[i].use = true;
        memory[i].start = i > 0 ? memory[i - 1].end + 1 : 0;
        memory[i].end =  memory[i].start + memory[used_hole_index[i]].size - 1;
        memory[i].size = memory[used_hole_index[i]].size;
        strcpy(memory[i].process, memory[used_hole_index[i]].process);
    }
    memory[cnt].use = false;
    memory[cnt].start = memory[cnt - 1].end + 1;
    memory[cnt].end = memory[cnt].start + unused - 1;
    memory[cnt].size = unused;
    for (int i = cnt + 1; i < MAX_HOLE_NUM; i++)
        memory[i].use = false;
    length = cnt + 1;
}

void report() {
    for (int i = 0; i < length; i++) {
        if (memory[i].use)
            printf("Addresses [%d:%d] Process %s\n", memory[i].start, memory[i].end, memory[i].process);
        else
            printf("Addresses [%d:%d] Unused\n", memory[i].start, memory[i].end);
    }
}