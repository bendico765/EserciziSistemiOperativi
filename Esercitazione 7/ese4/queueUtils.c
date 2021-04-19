#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct node{
	char *elem;
	struct node *nextNodePtr;
}node;
typedef struct node* nodePtr;

nodePtr generateNode(char *elem){
	nodePtr ptr = malloc(sizeof(node));
	if( ptr == NULL ){
		fprintf(stderr, "Node malloc error\n");
		exit(-1);
	}
	ptr->elem = elem;
	ptr->nextNodePtr = NULL;
	return ptr;
}

void insertNode(nodePtr *ptr, nodePtr newNode){
	if( *ptr == NULL ){
		*ptr = newNode;
	}
	else{
		nodePtr currentNode = *ptr;
		while( currentNode->nextNodePtr != NULL ){ currentNode = currentNode->nextNodePtr; }
		currentNode->nextNodePtr = newNode;
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
		printf("%s -> ", ptr->elem);
		printQueue(ptr->nextNodePtr);
	}
}
