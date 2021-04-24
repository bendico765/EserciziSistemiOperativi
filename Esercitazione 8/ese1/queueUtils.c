#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct node{
	int value;
	struct node *nextNodePtr;
}node;
typedef struct node* nodePtr;

nodePtr generateNode(int value){
	nodePtr ptr = malloc(sizeof(node));
	if( ptr == NULL ){
		printf("Node malloc error\n");
		fflush(stdout);
		exit(-1);
	}
	ptr->value = value;
	ptr->nextNodePtr = NULL;
	return ptr;
}

void insertNode(nodePtr *ptr, nodePtr newNode){
	if( *ptr == NULL ){
		*ptr = newNode;
	}
	else{
		nodePtr tempPtr = *ptr;
		while( tempPtr->nextNodePtr != NULL ){ tempPtr = tempPtr->nextNodePtr; }
		tempPtr->nextNodePtr = newNode;
	}
}

nodePtr removeNode(nodePtr *ptr){
	if( *ptr == NULL ){
		return NULL;
	}
	else{
		nodePtr headNode = *ptr;
		*ptr = (*ptr)->nextNodePtr;
		return headNode;
	}
}

int queueLen(nodePtr ptr){
	int len = 0;
	while( ptr != NULL ){ ptr = ptr->nextNodePtr; }
	return len;
}

void printQueue(nodePtr ptr){
	if( ptr == NULL ){
		printf("NULL\n");
	}
	else{
		printf("%2d -> ", ptr->value);
		printQueue(ptr->nextNodePtr);
	}
}
