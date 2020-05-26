//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <linux/unistd.h>
#include "functions.h"

void showUse();
void measure1(register int , register int ,register int *, register char *, register double *);



int main(int argc, char *argv[])
{
    int i, j, len, num;
    int a1[2];
    int s = 0;
    int size = 0;
    double *f, time1, min1=LARGE, min2=LARGE,ini_time;
    char message, ch;
    short num2;
    pid_t p = 0;
    unsigned long new_mask = 2;
    struct sched_param sp;

    len = sizeof(new_mask);
#ifdef MULTIPROCESSOR
    num = sched_setaffinity(p, len, &new_mask);
    if(num == -1)
    {
        perror("sched_setaffinity 1");
        exit(1);
    }
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    num = sched_setscheduler(0, SCHED_FIFO, &sp);
    if(num==-1)
    {
        perror("sched_setscheduler 1");
        exit(1);
    }
#endif

    while ((ch = getopt(argc, argv, "s:n:")) != EOF)
    {
        switch (ch)
        {
            case 'n':
                size = atoi(optarg);
                size = size/sizeof(double);
                break;
            case 's':
                s =atoi(optarg);
                s=s/sizeof(double);
                break;
            default:
                fprintf(stderr, "Unknown option character.\n");
                showUse();
                exit(1);
        }
    }
    
    if (s > size)
    {
        printf("Warning: stride is bigger than size of array. "
               "Sequential access. \n");
    }

    
    if (pipe (a1) < 0)
    {
        perror ("create a pipe");
        return -1;
    }
    
    printf("time1 without context switch: \t");
    fflush(stdout);

    for(num2=0; num2< N; num2++)
    {
        clean_cache();
        f = (double*) malloc(size*sizeof(double));
        if (f==NULL)
        {
            perror("malloc failed");
            exit (1);
        }
        memset((void *)f, 0x00, size*sizeof(double));
        sleep(1);
        ini_time = Time_in_sec();
        measure1(size, s, a1, &message, f);
        time1 = Time_in_sec()-ini_time;
        time1 = time1/NUM*MAX;
        if(min1 > time1)
            min1 = time1;
        printf("%f\t", time1);
        fflush(stdout);
        free_mem(f, size*sizeof(double));
        free(f);
        sleep(1);
    }
    printf("\nmeasure1: size of array = %lu, stride = %ld, min time1 = %.15f\n",size*sizeof(double), s*sizeof(double), min1);
    return 0;
}









void showUse()
{
  fprintf( stderr, "Usage:\nmeasure1 <options>\n-n   <number>  size of the array to work on (in byte). default 0.\n\n-s   <number>  access stride size (in byte). default 0\n");
}

void measure1(register int size, register int s,
    register int *a1, register char *c, register double *f)
{
    register int i, j, m;

    for ( i=0; i<NUM; i++)
    {
        for ( m=0; m<s; m++)
            for ( j=m; j<size; j=j+s)
                f[j]++;
        write(a1[1], c, 1);
        read(a1[0], c, 1);
    }
}
