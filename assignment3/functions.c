#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "functions.h"


double Time_in_sec(void)
{
    static double num=0;
    if (num==0) num= getCycles();
    return (cpu_cycle()*0.000001)/num;
}


time_frame cpu_cycle(void)
{
    unsigned int tmp[2];

    __asm__ ("rdtsc"
         : "=a" (tmp[1]), "=d" (tmp[0])
         : "c" (0x10) );
        
    return ( ((time_frame)tmp[0] << 32 | tmp[1]) );
}






double getCycles(void)
{
    double num = -1;
    char line[1024];
    char *s;
    char search_str[] = "cpu MHz";
    FILE *fp;
    
    if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
    {
        return -1;
    }
    
    while (fgets(line, 1024, fp) != NULL)
    {
        if (strstr(line, search_str) != NULL)
        {
            
            for (s = line; *s && (*s != ':'); ++s);
            if (*s && (sscanf(s+1, "%lf", &num) == 1))
                break;
        }
    }

    if(fp!=NULL)
    {
        fclose(fp);
    }
    
    return num;
}





void free_mem(double *n, int bytes)
{
    int var;

    if ((var = open("/dev/null",O_WRONLY)) == -1)
    {
        perror("/dev/null open error");
        exit(-1);
    }
    
    if (write(var, (void *)n, bytes) == -1)
    {
        perror("/dev/null write error");
        exit(-1);
    }
    
    if (close(var) != 0)
    {
        perror("/dev/null close error");
        exit(-1);
    }
}





void clean_cache()
{
    char *n;
    static char count = 0;
    n = (char*) malloc(SIZE);
    
    if (n == NULL)
    {
        perror("malloc fails");
        exit (1);
    }
    
    memset((void *)n, count++, SIZE);
    free(n);
}


