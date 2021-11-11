// gcc -pthread DQueue.c -o dqueue
#include <stdlib.h> // malloc
#include <pthread.h>// pthread_create,pthread_exit,pthread_yield

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

void Forall(DQueue* q, void* data, void (*callback)(void*, int)) {
  QNode* qn;
  pthread_mutex_lock(q->s); // wait to enter the critical section
  for(qn = q->left->right; qn != q->right; qn = qn->right)
    (*callback)(data, qn->val);
  pthread_mutex_unlock(q->s); // signal that we're done
}

int main() {
  // init 
  DQueue *q = (DQueue*)malloc(sizeof(DQueue));
  q->s = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(q->s, NULL);
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
  Forall(q, q, (void (*)(void*, int))&PushLeft); // blocks!

  // end all
  pthread_mutex_destroy(q->s);
  pthread_exit(NULL);
}
