#include "functions.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[])
{
    /*Required inputs check*/
    //if first argument is -h or -help display manual
    if(argc >= 2)
        if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1], "-help") == 0)
        {
            instruction_manual();
            exit(0);
        }
    int N=10; //Amount of cars
    int type=0; //0 for Mutex/Semaphore, 1 for Conditional Variables
    int debugmode = 0; //Test for debug mode, 0 for no debug mode, 1 for debug mode
    int choice;
    while((choice = getopt(argc, argv, "N:T:Dh:")) != -1)
    {
        switch(choice)
        {
            case 'N':
                N = atoi(optarg);
                if(N == 0)
                {
                    printf("Invalid argument for N\n");
                    printf("Valid argument is only a positive integer\n");
                    printf("Unless you wrote 0, in which case it will be set to default = 10, since there is no point of running the program with 0 cars.\n");
                    exit(1);
                }
                break;
            case 'T':
                type = atoi(optarg);
                if(type != 0 && type != 1)
                {
                    printf("Invalid variable -T. Please use '-T 0'for Mutexes and Semaphores, '-T 1' for Conditional Variables\n");
                    exit(1);
                }
                break;
            case 'D':
                debugmode = 1;
                break;
            default:
                printf("Wrong arguments\n");
                exit(1);
        }
    }
    /*Required inputs check/
    /*Debug test*/
    if(debugmode == 1)
    {
        printf("Debug mode enabled\n");
        printf("N: %d\n", N);
        printf("type: %d\n", type);
        exit(0);
    }
    /*Debug test*/
    /*Main Code*/
    if(type == 0)
    {

    }
    if(type == 1)
    {

    }
    /*Main Code*/
}
