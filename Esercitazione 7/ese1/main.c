/*
	Scrivere un programma C con due threads, un produttore (P) 
	ed un consumatore (C). Il thread P genera, uno alla volta, 
	una sequenza di numeri inserendoli in un buffer di una 
	sola posizione condiviso con il thread C. Il thread 
	consumatore estrae i numeri dal buffer e li stampa sullo 
	standard output. Se il buffer e' pieno P attende che C 
	consumi il dato, analogamente se il buffer e' vuoto C 
	attende che P produca un valore da consumare. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "queueUtils.h"
#include "threadUtils.h"
#define N 10

int rand_r(unsigned int *seedp);

static pthread_mutex_t myMtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;
static unsigned int seed;

void writeData(nodePtr *ptr){
	lock(&myMtx);
	while( *ptr != NULL ){
		wait(&myCond, &myMtx);
	}
	int value = rand_r(&seed)%100;
	nodePtr newPtr = generateNode(value);
	printf("Writing value %d into queue\n", value);
	fflush(stdin);
	insertNode(ptr, newPtr);
	signal(&myCond);
	unlock(&myMtx);
}

void readData(nodePtr *ptr){
	lock(&myMtx);
	while( *ptr == NULL ){
		wait(&myCond, &myMtx);
	}
	printf("Reading value %d from queue\n", (*ptr)->value);
	fflush(stdin);
	removeNode(ptr);
	signal(&myCond);
	unlock(&myMtx);
}

void* writeThread(void *arg){
	errno = 0;
	for(int i = 0; i < N; i++){ writeData((nodePtr*)arg); }
	pthread_exit((void*)0);
}

void* readThread(void *arg){
	errno = 0;
	for(int i = 0; i < N; i++){ readData((nodePtr*)arg); }
	pthread_exit((void*)0);
}

int main(int argc, char *argv[]){
	pthread_t tidWriter, tidReader;
	nodePtr queue = NULL;
	
	seed = time(NULL);
	pthread_create(&tidWriter, NULL, &writeThread, (void*)&queue);
	pthread_create(&tidReader, NULL, &readThread, (void*)&queue);
	
	pthread_join(tidWriter, NULL);
	pthread_join(tidReader, NULL);
	return 0;
}
