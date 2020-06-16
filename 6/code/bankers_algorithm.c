#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer_num */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer_num */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer_num */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);

int release_resources(int customer_num, int release[]);

bool check(bool Fin[], int Avail[], int Alloc[][NUMBER_OF_RESOURCES],int Need[][NUMBER_OF_RESOURCES]);

void show();

char cmd[5];
bool finished[NUMBER_OF_CUSTOMERS];

int main(int argc, char *argv[]) {
    assert(argc == 5);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        available[i] = atoi(argv[i + 1]);
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        finished[i] = false;
    FILE *f;
    f = fopen("maximum.txt", "r");

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            fscanf(f, "%d", &maximum[i][j]);
            fgetc(f);
        }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }

    int array[NUMBER_OF_RESOURCES];
    while (true) {
        printf("> ");
        scanf("%s", cmd);

        if (strcmp(cmd, "exit") == 0)
            break;
        if (strcmp(cmd, "*") == 0) {
            show();
            continue;
        }
        if ((strcmp(cmd, "RQ") != 0) && (strcmp(cmd, "RL") != 0))
        {
            printf("ERROR\n");
            continue;
        }
        int customer_num;
        int request[NUMBER_OF_RESOURCES];
        scanf(" %d", &customer_num);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
            scanf(" %d", &request[i]);
        if (strcmp(cmd, "RQ") == 0) {
            if (request_resources(customer_num, request) == -1)
                printf("request denied.\n");
            else
                printf("request accepted.\n");
        }
        else if (strcmp(cmd, "RL") == 0) {
            if (release_resources(customer_num, request) == -1)
                printf("release denied.\n");
            else
                printf("resource released.\n");
        }
    }
    fclose(f);
    return 0;
}

int request_resources(int customer_num, int request[]) {
    if (customer_num > NUMBER_OF_CUSTOMERS - 1) {
        printf("error customer id.\n");
        return -1;
    }
    if (finished[customer_num] == true)
    {
        printf("customer already finished.\n");
        return -1;
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        if (available[i] < request[i]) {
            printf("resource not enough!\n");
            return -1;
        }

    bool flag1 = true;
    bool flag2;

    bool finished_copy[NUMBER_OF_CUSTOMERS];
    int available_copy[NUMBER_OF_RESOURCES];
    int allocation_copy[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    int need_copy[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];


    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finished_copy[i] = finished[i];
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation_copy[i][j] = allocation[i][j];
            need_copy[i][j] = need[i][j];
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available_copy[i] = available[i] - request[i];
        allocation_copy[customer_num][i] += request[i];
        need_copy[customer_num][i] -= request[i];
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        if (need_copy[customer_num][i] != 0)
        {
            flag1 = false;
            break;
        }
    if (flag1)
    {
        finished_copy[customer_num] = true;
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
            available_copy[i] += allocation_copy[customer_num][i];
    }

    flag2 = check(finished_copy, available_copy, allocation_copy, need_copy);
    if (flag2) {
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] -= request[i];
            allocation[customer_num][i] += request[i];
            need[customer_num][i] -= request[i];
        }
        if (flag1)
            finished[customer_num] = true;
        printf("safe state\n");
        return 0;
    }
    else
    {
        printf("unsafe state\n");
        return -1;
    }
}

int release_resources(int customer_num, int release[]) {
    if (customer_num > NUMBER_OF_CUSTOMERS - 1) {
        printf("error customer id.\n");
        return -1;
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        if (allocation[customer_num][i] < release[i]) {
            printf("release too much!\n");
            return -1;
        }
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
    return 0;
}

bool check(bool Fin[], int Avail[], int Alloc[][NUMBER_OF_RESOURCES],int Need[][NUMBER_OF_RESOURCES]) {
    bool flag = false;
    bool non_finish = false;
    bool solve = false;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        if (!Fin[i])
        {
            non_finish = true;
            break;
        }
    if (!non_finish)
        return true;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (Fin[i])
            continue;
        flag = false;
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            if (Avail[j] < Need[i][j]) {
                flag = true;
                break;
            }
        if (flag)
            continue;

        solve = true;
        Fin[i] = true;
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
            Avail[j] += Alloc[i][j];
        break;
    }
    if (!solve)
        return false;
    if (check(Fin, Avail, Alloc, Need))
        return true;
    else
        return false;
}

void show() {
    printf("Available:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        printf("Resource%d: %d\t", i, available[i]);
    printf("\n");

    printf("Maximum:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer%d:\t", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("Resource%d: %d\t", j, maximum[i][j]);
        }
        printf("\n");
    }

    printf("Allocation:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (finished[i] == true)
            continue;
        printf("Customer%d:\t", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("Resource%d: %d\t", j, allocation[i][j]);
        }
        printf("\n");
    }

    printf("Need:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (finished[i] == true)
            continue;
        printf("Customer%d:\t", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("Resource%d: %d\t", j, need[i][j]);
        }
        printf("\n");
    }
}