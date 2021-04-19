/*
	Scrivere un programma C che implementa una pipeline di tre 
	threads. 
	Il primo thread legge una riga alla volta da un file testuale 
	(il cui nome e' passato come argomento al programma) ed invia 
	al secondo thread ogni riga letta dal file. 
	Il secondo thread “tokenizza” le parole dalla riga ricevuta dal 
	primo thread (considerare come separatore solo lo spazio) ed 
	invia al terzo thread le parole. 
	Il terzo thread stampa tutte le parole sullo standard output. 
	I tre stadi della pipeline devono lavorare in modo concorrente 
	come in una “catena di montaggio”, il buffer di comunicazione 
	tra due stadi della pipeline deve essere implementata con una 
	coda FIFO (la scelta se usare una coda di capacita bounded o 
	unbounded e' lasciata allo studente). 
	
	Estendere l'esercizio in modo che il terzo thread della 
	pipeline, invece di stampare tutte le parole ricevute dallo 
	stadio intermedio della pipeline, stampi sullo standard 
	output ogni occorrenza di una parola una sola volta
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "queueUtils.h"
#include "threadUtils.h"
#define BUFFERSIZE 256
#define ce_null(var,msg) if( var == NULL ){ perror(msg);exit(-1); }

//locks e cond. var. della coda delle righe lette
pthread_mutex_t rowsQueueMtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rowsQueueCond = PTHREAD_COND_INITIALIZER;
//locks e cond.var della coda di parole tokenizzate
pthread_mutex_t wordsQueueMtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wordsQueueCond = PTHREAD_COND_INITIALIZER;

struct ArgStruct{
	nodePtr *rowsQueue;
	nodePtr *wordsQueue;
	FILE *inputFile;
};

void* readerThread(void *arg){
	FILE *inputFile = ((struct ArgStruct*)arg)->inputFile;
	nodePtr *rowsQueue = ((struct ArgStruct*)arg)->rowsQueue;
	char buffer[BUFFERSIZE];
	errno = 0;
	
	while( fgets(buffer, BUFFERSIZE, inputFile) != NULL ){
		char *elem = strdup((const char*)buffer);
		ce_null(elem, "Errore nell'allocazione della strdup");
		nodePtr newNode = generateNode(elem);
		// regione critica in cui inserisco la riga nella coda
		lock(&rowsQueueMtx);
		insertNode(rowsQueue, newNode); 
		// notifico l'eventuale tokenizer in attesa
		signal(&rowsQueueCond); 
		unlock(&rowsQueueMtx);
	}
	// ho terminato la lettura: lo segnalo al tokenizer
	// spedendogli la stringa null
	nodePtr terminationNode = generateNode(NULL);
	lock(&rowsQueueMtx);
	insertNode(rowsQueue, terminationNode); 
	signal(&rowsQueueCond); 
	unlock(&rowsQueueMtx);
	pthread_exit(0);
}

void* tokenizerThread(void *arg){
	nodePtr *rowsQueue = ((struct ArgStruct*)arg)->rowsQueue;
	nodePtr *wordsQueue = ((struct ArgStruct*)arg)->wordsQueue;
	int terminationFlag = 0;
	errno = 0;
	while( terminationFlag == 0 ){
		nodePtr removedNode;
		// regione critica in cui prelevo una riga dalla coda
		// se non c'è nulla da prelevare, attendo
		lock(&rowsQueueMtx);
		while( *rowsQueue == NULL ){
			wait(&rowsQueueCond, &rowsQueueMtx);
		}
		removedNode = removeNode(rowsQueue);
		unlock(&rowsQueueMtx);
		//faccio il tokenizer
		if( removedNode->elem != NULL ){
			char *saveptr = NULL;
			char *token = strtok_r(removedNode->elem, " ", &saveptr);
			while( token != NULL ){
				// incapsulo il token e lo spedisco al printer
				char *elem = strdup((const char*)token);
				ce_null(elem, "Errore nell'allocazione della strdup");
				nodePtr newNode = generateNode(elem);
				lock(&wordsQueueMtx);
				insertNode(wordsQueue, newNode);
				signal(&wordsQueueCond);
				unlock(&wordsQueueMtx);
				token = strtok_r(NULL, " ", &saveptr);
			}
			free(removedNode->elem);
		}
		else{ terminationFlag = 1; }
		free(removedNode);
	}
	// ho terminato: lo segnalo al printer
	// spedendogli la stringa null
	nodePtr terminationNode = generateNode(NULL);
	lock(&wordsQueueMtx);
	insertNode(wordsQueue, terminationNode); 
	signal(&wordsQueueCond); 
	unlock(&wordsQueueMtx);
	pthread_exit(0);
}
void* printerThread(void *arg){
	nodePtr *wordsQueue = ((struct ArgStruct*)arg)->wordsQueue;
	errno = 0;
	int terminationFlag = 0;
	
	while(  terminationFlag == 0){
		lock(&wordsQueueMtx);
		while( *wordsQueue == NULL ){
			wait(&wordsQueueCond, &wordsQueueMtx);
		}
		nodePtr removedNode = removeNode(wordsQueue);
		unlock(&wordsQueueMtx);
		if( removedNode->elem != NULL ){
			printf("|%s", removedNode->elem);
			free(removedNode->elem);
		}
		else{
			terminationFlag = 1;
		}
		free(removedNode);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]){
	nodePtr rowsQueue = NULL; // coda contenente le righe lette dal reader ed usate dal tokenizer
	nodePtr wordsQueue = NULL; // coda contenete le parole ottenute dal tokenizer e stampate dal printer
	FILE *inputFile = NULL;
	pthread_t tidReader, tidTokenizer, tidPrinter;
	
	errno = 0;
	if( argc != 2 ){
		fprintf(stderr, "Wrong parameters, use %s [pathname]\n", argv[0]);
		return -1;
	}
	if( (inputFile = fopen(argv[1], "r")) == NULL ){
		perror("Error while opening file");
		return -1;
	}
	
	printf("File aperto con successo!\n");
	struct ArgStruct arg1 = {&rowsQueue, NULL, inputFile};
	struct ArgStruct arg2 = {&rowsQueue, &wordsQueue, NULL};
	struct ArgStruct arg3 = {NULL, &wordsQueue, NULL};
	
	pthread_create(&tidReader, NULL, &readerThread, (void*)&arg1);
	pthread_create(&tidTokenizer, NULL, &tokenizerThread, (void*)&arg2);
	pthread_create(&tidPrinter, NULL, &printerThread, (void*)&arg3);
	
	pthread_join(tidReader, NULL);
	pthread_join(tidTokenizer, NULL);
	pthread_join(tidPrinter, NULL);
	fclose(inputFile);
	return 0;
}
