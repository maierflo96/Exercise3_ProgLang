// gcc -pthread bumpersemaphore.c -o bumpersemaphore
#include <pthread.h> // pthread_create, pthread_exit
#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <semaphore.h>
#define NUM_THREADS 2

char heap[1<<30];
char* firstFree = &heap[0];
sem_t mutex;

char* myalloc(int size) {
  char* start;

  sem_wait(&mutex);
  
  start= firstFree;
  firstFree = firstFree + size;

  sem_post(&mutex);
  
//if (start+size>sizeof(heap)) garbage_collect();
  if (*start+size>(sizeof(heap)/sizeof(heap[0]))) ;
 
  return start;
}

void *allocator(void *threadid){
  for (int i=0;i<100000000;i++)
    myalloc(1);
  
}


int main(int argc, char *argv[]) {
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;

  sem_init(&mutex,0,1);
  
  for(t = 0; t < NUM_THREADS; t++) {
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, allocator, (void *)t);
    if(rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  for(t = 0; t < NUM_THREADS; t++) {
    pthread_join(threads[t],NULL);
  }

  sem_destroy(&mutex);
  printf(" %ld\n",firstFree-&heap[0]);
  
  /* last thing that main() should do */
  pthread_exit(NULL);
}
