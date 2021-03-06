/* On-demand 2-thread synchronization */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "gcc cv-sync-1.c -o cv-sync-1 -lpthread"

void exec_Thread1(long *tid1);
void exec_Thread2(long *tid2);

int flag = 0,
    total_runs = 0,
    t1_runs = 0,
    t2_runs = 0;

int global_index_run = 1,
    t1_ran = 0,
    t2_ran = 0;

long *tid1, *tid2;

void exec_Thread1(long *tid1)
{
  if(flag == 0)
  {
    for(int i = 0; i < t1_runs; i++)
    {
      if(total_runs == 0)
          break;
      printf("%d: In thread T1, TID1=%ld\n", global_index_run, tid1);
      global_index_run++;
      printf("GLOBAL=%d\n", global_index_run);
      t1_ran++;
      total_runs--;
    }
    flag = 1;
  }
  else
  {
    printf("Invalid Input\n");
  }
}

void exec_Thread2(long *tid2)
{
  if(flag == 1)
  {
    for(int i = 0; i < t2_runs; i++)
    {
      if(total_runs == 0)
          break;
      printf("%d: In thread T2, TID2=%ld\n", global_index_run, tid2);
      global_index_run++;
      printf("GLOBAL=%d\n", global_index_run);
      t2_ran++;
      total_runs--;
    }
    flag = 0;
  }
  else
  {
    printf("Invalid Input\n");
  }
}

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c1 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t c2 = PTHREAD_COND_INITIALIZER;

void *myThread1(void *arg)
{
  tid1 = (long*)arg;
  while(total_runs > 0)
  {
    pthread_mutex_lock(&lock);
    while(flag == 1)
    {
      pthread_cond_wait(&c1,&lock);
    }
    exec_Thread1(tid1);
    pthread_cond_signal(&c2);
    pthread_mutex_unlock(&lock);
  }
}

void *myThread2(void *arg)
{
  tid2 = (long*)arg;
  while(total_runs > 0)
  {
    pthread_mutex_lock(&lock);
    while(flag == 0)
    {
      pthread_cond_wait(&c2,&lock);
    }
    exec_Thread2(tid2);
    pthread_cond_signal(&c1);
    pthread_mutex_unlock(&lock);
  }
}

void usage(char * bin_str) {
    printf("Usage: %s total_runs t1_runs t2_runs\n", bin_str);
}

int main(int argc, char * argv[]) {

    if (argc != 4) {
        usage(argv[0]);
        return 0;
    }
    total_runs = atoi(argv[1]);
    t1_runs = atoi(argv[2]);
    t2_runs = atoi(argv[3]);

    pthread_t t1,t2;

    pthread_create(&t1,NULL,myThread1,(void *)&t1);
    pthread_create(&t2,NULL,myThread2,(void *)&t2);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    printf("In main: T1 printed %d times, T2 printed %d times.\n",t1_ran,t2_ran);

    return 0;
}
