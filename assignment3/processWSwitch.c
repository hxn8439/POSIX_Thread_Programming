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
#include <sys/stat.h>
#include <fcntl.h>
#include "functions.h"

void showUse();
void measureSwitch1(register int, register int,register int*, register int*, register char*,
                    register double*);
void measureSwitch2(register int, register int,register int*, register int*, register char*,
                    register double*);




int main(int argc, char *argv[])
{
    int i, j, len, num2, p1[2], p2[2], stride=0, arrays=0;
    double *f, start_time, time1, min2=LARGE;
    char message, ch;
    short round;
    pid_t pid;
    pid_t p = 0;
    unsigned long mask = 2;
    struct sched_param sp;

    len = sizeof(mask);
#ifdef MULTIPROCESSOR
    num2 = sched_setaffinity(p, len, &mask);
    if(num2==-1)
    {
        perror("sched_setaffinity 1");
        exit(1);
    }
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    num2=sched_setscheduler(0, SCHED_FIFO, &sp);
    if(num2==-1)
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
                arrays=atoi(optarg);
                arrays=arrays/sizeof(double);
                break;
            case 's':
                stride=atoi(optarg);
                stride=stride/sizeof(double);
                break;
            default:
                fprintf(stderr, "Unknown option character.\n");
                showUse();
                exit(1);
        }
    }
    
    if (stride > arrays)
    {
        printf("Warning: stride is bigger than the size of array. "
               "Sequential access. \n");
    }


    if (pipe (p1) < 0)
    {
        perror ("create pipe1");
        return -1;
    }
    
    if (pipe (p2) < 0)
    {
        perror ("create pipe2");
        return -1;
    }
   
    
    printf("time2 with context swith: \t");
    fflush(stdout);
    for(round=0; round<N; round++)
    {
    
        clean_cache();
        if ((pid = fork()) <0)
        {
            perror("fork");
            return -1;
        }
        else if (pid ==0)
        {
            f = (double*) malloc(arrays*sizeof(double));
            if (f==NULL)
            {
                perror("malloc fails");
                exit (1);
            }
            memset((void *)f, 0x00, arrays*sizeof(double));
            measureSwitch1(arrays, stride, p1, p2, &message, f);
            sleep(1);
            free_mem(f, sizeof(double)*arrays);
            free(f);
            exit(0);
        }
        else
        {
            f = (double*) malloc(arrays*sizeof(double));
            if (f==NULL)
            {
                perror("malloc fails");
                exit (1);
            }
            memset((void *)f, 0x00, arrays*sizeof(double));
            sleep(1);
            start_time = Time_in_sec();
            measureSwitch2(arrays, stride, p1, p2, &message, f);
            time1 = Time_in_sec()-start_time;
            time1 = time1/(2*NUM)*MAX;
            if(min2 > time1)
                min2 = time1;
            
            printf("%f\t", time1);
            fflush(stdout);
            waitpid(pid, NULL, 0);
            
            free_mem(f, sizeof(double)*arrays);
            free(f);
        }
        sleep(1);
    }
    
    printf("\nmeasureSwitch: size of array = %lu, stride = %ld, min time2 = %.15f\n",
    arrays*sizeof(double), stride*sizeof(double), min2);
    return 0;
}




void showUse()
{
  fprintf( stderr, "Usage:\nmeasureSwitch <options>\n\
    -n   <number>  size of the array to work on (in byte). default 0.\n\
    -s   <number>  access stride size (in byte). default 0\n");
}

void measureSwitch1(register int size, register int s,
    register int *a1, register int *a2, register char *c,
    register double *f)
{
    register int i, j, m;

    for ( i=0; i<NUM; i++)
    {
        read(a2[0], c, 1);
        for ( m=0; m<s; m++)
            for ( j=m; j<size; j=j+s)
                f[j]++;
        write(a1[1], c, 1);
    }
}



void measureSwitch2(register int size, register int s,
    register int *a1, register int *a2, register char *c,
    register double *f)
{
    register int i, j, m;

    for ( i=0; i<NUM; i++)
    {
        for ( m=0; m<s; m++)
            for ( j=m; j<size; j=j+s)
                f[j]++;
        write(a2[1], c, 1);
        read(a1[0], c, 1);
    }

}
