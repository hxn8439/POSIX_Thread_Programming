#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define BUFFERSIZE 5

typedef struct data
{
	int threadNum;
	int partCount;
}datum;

char bufferarray[BUFFERSIZE];
int end;
int a,b;

pthread_cond_t empty, full;
pthread_mutex_t mutex;


void producer(FILE *fp)
{
	
	char start;
	
	start = fgetc(fp);
	
	while(1)
	{
		pthread_mutex_lock(&mutex);
		bufferarray[a] = start;
		a = (a+1)%BUFFERSIZE;
		
		if((a-1)%BUFFERSIZE == b)
		{
			pthread_cond_signal(&empty);
			
			//testing printout
			//printf("\n The signal is currently empty\n");
		}

		if(a == b)
		{
			//testing printout
			//printf("producer is in current sleep\n");
			
			pthread_cond_wait(&full, &mutex);
			
			//testing printout
			//printf("producer is currently awaken\n");
		}	
		
		pthread_mutex_unlock(&mutex);
		
		start =fgetc(fp);
		if(feof(fp))
		{
			pthread_mutex_lock(&mutex);
            end = 1;
            pthread_cond_signal(&empty);
			
			//testing printout
			//printf("signal is going to end\n");
			
			pthread_mutex_unlock(&mutex);
			break;
		}			
	}	
}		

void consumer(void *ptr)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(a == b)
		{
			if(end) 
			{	
				break;
			}

			else
			{
				
				//testing printout
				//printf("consumer is sleep or ending\n");
                
				pthread_cond_wait(&empty, &mutex);
                if(a == b && end) 
				{	
					break;
				}
			}
		}
		
		printf("%c",bufferarray[b]);
		b = (b+1)%BUFFERSIZE;
		
		if(a == (b-1)%BUFFERSIZE)
		{
			pthread_cond_signal(&full);
			
			//testing printout
			//printf("signal is full for consumer\n");
		}

		pthread_mutex_unlock(&mutex);
	}	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t produce;
	pthread_t consume;
	
	FILE *fp;
	
	a = 0;
	b = 0;
	
	if((fp=fopen("message.txt", "r")) == NULL)
	{
		printf("ERROR: cannot open message.txt!\n");
		return -1;
	}
	
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&empty, NULL);
    pthread_cond_init (&full, NULL);
    
    /* create threads 1 and 2 */    
    pthread_create (&produce, NULL, (void *) &producer, (FILE *) fp);
    pthread_create (&consume, NULL, (void *) &consumer, NULL);

    pthread_join(produce, NULL);
    pthread_join(consume, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);
    
	printf("\n");
	
	return 1;
}



