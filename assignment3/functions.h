#define SIZE    4194304
#define NUM    10000
#define MULTIPROCESSOR
#define N       3
#define MAX 1000000
#define LARGE   100000000

typedef long long time_frame;


double Time_in_sec(void);

time_frame cpu_cycle(void);

double getCycles(void);

void free_mem(double *n, int bytes);

void clean_cache();
