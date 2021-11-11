// gcc -pthread DQueueMonitor.c -o dqueue
#include <stdlib.h> // malloc
#include <stdio.h>  // printf
#include <pthread.h>// pthread_create,pthread_exit,pthread_yield
#define RUNS 1000000
#define N 20

typedef struct QNode {
  int val;
  struct QNode* left;
  struct QNode* right;
} QNode;

typedef struct {
  pthread_mutex_t *s;
  struct QNode* left;
  struct QNode* right;
} DQueue;

DQueue *q;

void PushLeft(DQueue* q, int val) {
  QNode *qn = (QNode *)malloc(sizeof(QNode));
  qn->val = val;

  pthread_mutex_lock(q->s); // wait to enter the critical section
  QNode* leftSentinel = q->left;
  QNode* oldLeftNode = leftSentinel->right;
  qn->left = leftSentinel;
  qn->right = oldLeftNode;
  leftSentinel->right = qn;
  oldLeftNode->left = qn;
  pthread_mutex_unlock(q->s); // signal that we're done
}

int PopRight(DQueue* q) {
  QNode* oldRightNode;
  QNode* leftSentinel = q->left;

  pthread_mutex_lock(q->s); // wait to enter the critical section
  QNode* rightSentinel = q->right;
  oldRightNode = rightSentinel->left;
  if(oldRightNode == leftSentinel) {
    pthread_mutex_unlock(q->s);
    return -1;
  }
  QNode* newRightNode = oldRightNode->left;
  newRightNode->right = rightSentinel;
  rightSentinel->left = newRightNode;
  pthread_mutex_unlock(q->s); // signal that we're done
  int ret = oldRightNode->val;
  free(oldRightNode);
  return ret;
}
;

void Forall(DQueue* q, void* data, void (*callback)(void*, int)) {
  QNode* qn;
  pthread_mutex_lock(q->s); // wait to enter the critical section
  for(qn = q->left->right; qn != q->right; qn = qn->right)
    (*callback)(data, qn->val);
  pthread_mutex_unlock(q->s); // signal that we're done
}

void *Thread(void *threadid) {
  long tid = (long)threadid;
  long i, j;
  for(i = 0; i < RUNS; i++) {
    for(j = 0; j < N + 2 * tid; j++) {
      PushLeft(q, (int)tid);
    }
    for(j = 0; j < N + 2 * tid; j++) {
      PopRight(q);
    }
  }

  printf("Thread #%ld finished\n", tid);
  pthread_exit(NULL);
}

int main() {
  // init 
  long t, rc;
  q = (DQueue*)malloc(sizeof(DQueue));
  q->s = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  // start monitor specific code
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  /*
   * if NULL == &attr then a lock instead of a monitor
   */
  pthread_mutex_init(q->s, &attr);
  // end monitor specific code
  QNode* sentinel = (QNode*)malloc(sizeof(QNode));
  q->right = sentinel;
  q->left = sentinel;
  sentinel->right = sentinel;
  sentinel->left = sentinel;

  // fill initial load
  PushLeft(q, 1);
  PushLeft(q, 2);
  PushLeft(q, 3);
  // Forall
  Forall(q, q, (void (*)(void*, int))&PushLeft);  // blocks!
  pthread_t threads[2];
  for(t = 0; t < 2; t++) {
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, Thread, (void *)t);
    if(rc) {
      printf("ERROR; return code from pthread_create() is %ld\n", rc);
      exit(-1);
    }
  }

  // end all
  //  pthread_mutex_destroy(q->s);
  //free(q);
  pthread_exit(NULL);
}
;
