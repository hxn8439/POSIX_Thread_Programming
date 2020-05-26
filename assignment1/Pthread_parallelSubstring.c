#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 1024
#define NUMTHREAD 4

int total = 0;
int n1, n2; 
char *s1, *s2;
FILE *fp;

typedef struct data
{
	int threadNum;
	int partialCount;
} datum;	

int readf(FILE *fp)
{
	if((fp=fopen("string.txt","r"))==NULL)
	{
		printf("ERROR: can't open string.txt!\n");
		return 0;
	}
	
	s1=(char *)malloc(sizeof(char)*MAX);
	if(s1==NULL)
	{
		printf("ERROR: Out of memory!\n");
		return -1;
	}
	
	s2=(char *)malloc(sizeof(char)*MAX);
	if(s2==NULL)
	{
		printf("ERROR: Out of memory!\n");
		return -1;
	}
	
	/*read s1, s2 from the file */
	s1 = fgets(s1, MAX, fp);
	s2 = fgets(s2, MAX, fp);
	n1 = strlen(s1); /*length of s1 */
	n2 = strlen(s2)-1; /*length of s2 */
	if(s1==NULL || s2==NULL || n1<n2) /*when error exit*/
		return -1;
}

int num_substring(void)
{
	int i, j, k;
	int count;
	
	for(i =0; i <=(n1-n2); i++)
	{
		count = 0;
		for(j = i, k = 0; k < n2; i++, k++)
		{
			if(*(s1+j) != *(s2+k))
			{
				break;
			}
			
			else
				count++;
			
			if(count==n2)
				total++;
			
			
		}		
	}
	return total;
}
	
void ptr_substring_num(void *ptr)
{
	int i, j , k;
	int count;
	int total = 0;
	
	for(i =((datum*) ptr)->threadNum; i <= (n1-n2); i = i + NUMTHREAD)
	{
		count = 0;
		
		for (j = i, k= 0; k < n2; j++, k++)
		{
			if (*(s1+j)!=*(s2+k))
			{
				break;
			}

			else
				count++;

			if(count==n2)
				total++;
		}	

	}
	
	((datum*)ptr) ->partialCount = total;
}	

	
int main(int argc, char *argv[])
{
	int count;
	int partialSumCount;
	
	pthread_t thread1, thread2, thread3, thread4;
	datum datum1, datum2, datum3, datum4;
	
	datum1.threadNum =1;
	datum2.threadNum =2;
	datum3.threadNum =3;
	datum4.threadNum =4;
	
	pthread_create (&thread1, NULL, (void *) &ptr_substring_num, (void *) &datum1);
	pthread_create (&thread2, NULL, (void *) &ptr_substring_num, (void *) &datum2);
	pthread_create (&thread3, NULL, (void *) &ptr_substring_num, (void *) &datum3);
	pthread_create (&thread4, NULL, (void *) &ptr_substring_num, (void *) &datum4);
	
	readf(fp);
	count = num_substring ();
	
	partialSumCount = 0;
	
	pthread_join(thread1, NULL);
	partialSumCount += datum1.partialCount;
	
	pthread_join(thread2, NULL);
	partialSumCount += datum2.partialCount;
	
	pthread_join(thread3, NULL);
	partialSumCount += datum3.partialCount;
	
	pthread_join(thread4, NULL);
	partialSumCount += datum4.partialCount;
	
	printf("The number of substrings is %d\n", count);
	printf("The number of substrings counted in parallel is: %d\n", partialSumCount);
	return 1;
}
	
