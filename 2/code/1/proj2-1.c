#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LINE 80 /* The maximum length command */

char *args[MAX_LINE/2 + 1]; /* command line arguments */
char *history_args[MAX_LINE/2 + 1];
char *pipe_args[MAX_LINE/2 + 1];
char *filename;
char temp[20];
int should_run = 1; /* flag to determine when to exit program */
int pos;
int history_pos;
int pipe_pos;
char ch;
char str[MAX_LINE][20];
int wait_flag;
int red_flag; //0为不需要重定向，1为输出重定向，2为输入重定向
int pipe_flag;
int fd;
pid_t pid;
pid_t pipe_pid;

int main(void)
{
    while (should_run) {
        usleep(10000);
        printf("osh>");
        fflush(stdout);

        for (int i = 0; i < MAX_LINE/2 + 1; i++) //初始化清空args
            args[i] = NULL;
        //初始化变量
        pos = 0;
        pipe_pos = 0;
        wait_flag = 1;
        red_flag = 0;
        pipe_flag = 0;
        //循环读入指令
        while (scanf("%s", str[pos]))
        {
            args[pos] = str[pos];
            scanf("%c", &ch);
            pos++;
            if(ch == '\n')
                break;
        }
        // 判断结束
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            continue;
        }
        // 判断历史指令
        if (strcmp(args[0], "!!") != 0)
        {
            for (int i = 0; i < pos; i++)
            {
                strcpy(temp, args[i]);
                history_args[i] = temp;
            }
            history_pos = pos;
        }
        else{
            if (history_args[0] == NULL) {
                printf("NO commands in history.\n");
                continue;
            }
            else {
                for (int i = 0; i < history_pos; i++) {
                    strcpy(temp, history_args[i]);
                    args[i] = temp;
                }
                pos = history_pos;
            }
        }
        //判断wait
        if (strcmp(args[pos - 1], "&") == 0)
        {
            wait_flag = 0;
            args[pos - 1] = NULL;
            pos--;
        }
        //判断重定向
        if (pos>=2 && strcmp(args[pos-2],">")==0)
        {
            red_flag = 1;
            strcpy(temp, args[pos-1]);
            filename = temp;
            args[pos-1] = NULL;
            pos--;
            args[pos-1] = NULL;
            pos--;
        }
        else if (pos>=2 && strcmp(args[pos-2],"<")==0)
        {
            red_flag = 2;
            strcpy(temp, args[pos-1]);
            filename = temp;
            args[pos-1] = NULL;
            pos--;
            args[pos-1] = NULL;
            pos--;
        }
        //判断 pipe
        for (int i = 0; i < pos; i++)
        {
            if (strcmp(args[i], "|") == 0)
            {
                pipe_flag = 1;
                args[i] = NULL;

                for (int j = i + 1; j < pos; j++)
                {
                    strcpy(temp, args[j]);
                    pipe_args[pipe_pos] = temp;
                    args[j] = NULL;
                    pipe_pos++;
                }
                pos -= pipe_pos;
                break;
            }
        }
        //执行
        pid = fork();
        if (pid==0)//子进程
        {
            //建立重定向
            if (red_flag == 1)
            {
                fd = open(filename,O_CREAT|O_RDWR,S_IRWXU);
                dup2(fd, STDOUT_FILENO);
            }
            else if (red_flag == 2)
            {
                fd = open(filename,O_CREAT|O_RDONLY,S_IRUSR);
                dup2(fd, STDIN_FILENO);
            }
            //建立通信
            if(pipe_flag)
            {
                pipe_pid = fork();
                if(pipe_pid == 0)// 子进程执行pipe_arg
                {
                    usleep(500000);
                    fd = open("pipe.txt",O_CREAT|O_RDONLY,S_IRUSR);
                    dup2(fd, STDIN_FILENO);
                    execvp(pipe_args[0], pipe_args);
                    exit(0);
                }
                else// 父进程
                {
                    fd = open("pipe.txt",O_CREAT|O_RDWR,S_IRWXU);
                    dup2(fd, STDOUT_FILENO);
                    execvp(args[0], args);
                    wait(NULL);
                }
            }
            else
                execvp(args[0], args);
            exit(0);
        }
        else//父进程
        {
            if(wait_flag)
                wait(NULL);
            else
                signal(SIGCHLD, SIG_IGN);
        }
/**
* After reading user input, the steps are:
* (1) fork a child process using fork()
* (2) the child process will invoke execvp()
* (3) parent will invoke wait() unless command included &
*/
    }
    return 0;
}
