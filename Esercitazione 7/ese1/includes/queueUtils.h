#ifndef QUEUE_UTILS_H
#define QUEUE_UTILS_H
#endif

struct node{
	int value;
	struct node *nextNodePtr;
}node;
typedef struct node* nodePtr;

nodePtr generateNode(int value);
void insertNode(nodePtr *ptr, nodePtr newNode);
int removeNode(nodePtr *ptr);
void printQueue(nodePtr ptr);
int queueLen(nodePtr ptr);
