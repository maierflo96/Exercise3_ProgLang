// gcc Dqueue_pure.c -o dqueue
#include <stdlib.h> // malloc
typedef struct QNode {
  int val;
  struct QNode* left;
  struct QNode* right;
} QNode;
typedef struct {
  struct QNode* left;
  struct QNode* right;
} DQueue;

void PushLeft(DQueue* q, int val) {
  QNode *qn = (QNode *)malloc(sizeof(QNode));
  qn->val = val;
  QNode* leftSentinel = q->left;
  QNode* oldLeftNode = leftSentinel->right;
  qn->left = leftSentinel;
  qn->right = oldLeftNode;
  leftSentinel->right = qn;
  oldLeftNode->left = qn;
}

int PopRight(DQueue* q) {
  QNode* oldRightNode;
  QNode* leftSentinel = q->left;
  QNode* rightSentinel = q->right;
  oldRightNode = rightSentinel->left;
  if(oldRightNode == leftSentinel)
    return -1;
  QNode* newRightNode = oldRightNode->left;
  newRightNode->right = rightSentinel;
  rightSentinel->left = newRightNode;
  int ret = oldRightNode->val;
  free(oldRightNode);
  return ret;
}

void Forall(DQueue* q, void* data, void (*callback)(void*, int)) {
  // Executes callback on all items of this list
}

int main() {
  // init 
  DQueue *q = (DQueue*)malloc(sizeof(DQueue));
  QNode* sentinel = (QNode*)malloc(sizeof(QNode));
  q->right = sentinel;
  q->left = sentinel;
  sentinel->right = sentinel;
  sentinel->left = sentinel;

  // fill initial load
  
  // ... code to prepare a deadlock

  // Forall

  // ... produce a deadlock

  // end all
}
