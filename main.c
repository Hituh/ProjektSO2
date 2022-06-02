#include "functions.h"
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
    
    /*------------------------------------Required inputs check------------------------------------*/
    //if first argument is -h or -help display manual
    if(argc >= 2)
        if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1], "-help") == 0)
        {
            instruction_manual();
            exit(0);
        }
    int choice;
    int N=10; //Amount of cars
    int type=0; //0 for Mutex/Semaphore, 1 for Condition Variable
    while((choice = getopt(argc, argv, "N:T:")) != -1)
    {
        switch(choice)
        {
            case 'N':
                N = atoi(optarg);
                break;
            case 'T':
                type = atoi(optarg);
                if(type != 0 && type != 1)
                {
                    printf("Invalid variable -T. Please use '-T 0'for Mutexes and Semaphores, '-T 1' for Conditional Variables\n");
                    exit(1);
                }
                break;
            default:
                printf("Wrong arguments\n");
                exit(1);
        }
    }
    printf("N: %d\n", N);
    printf("type: %d\n", type);
    /*------------------------------------Required inputs check------------------------------------*/
}
