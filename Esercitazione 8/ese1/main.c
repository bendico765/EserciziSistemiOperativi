/*
	Scrivere un programma C in cui si attivano M thread produttori
	ed N thread consumatori che condividono una coda (di capacità 
	“infinita”). 
	Il programma accetta come argomento anche un intero K che 
	corrisponde al numero totale di messaggi che i produttori 
	producono concorrentemente nella coda (ogni produttore 
	produce K/M messaggi – se M divide K). 
	I consumatori leggono i messaggi dalla coda in un ciclo 
	infinito fino a quando non ricevono un messaggio speciale 
	che li fa terminare. Implementare la coda concorrente, ed 
	il protocollo di terminazione senza usare la cancellazione 
	esplicita dei threads. Testare il programma al variare di 
	M ed N. 
	Controllare con valgrind che non ci siano problemi 
	nell'accesso alle variabili del programma e che tutta la 
	memoria allocata sia completamente rilasciata all'uscita 
	del programma. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "queueUtils.h"
#include "threadUtils.h"
#include "utils.h"
#define ce_ne0(var,msg) if( (var) != 0 ){perror(msg); exit(-1);}
#define ce_less1(var,msg) if( (var) == -1 ){perror(msg); exit(-1);}

static unsigned int seed;
// mutex e cond.var per l'accesso alla coda condivisa
static pthread_mutex_t myMtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;

int rand_r(unsigned int *seedp);

struct Args{
	// numbero di messaggi che il thread deve mandare
	int numberOfMessages; 
	// numero di messaggi di terminazione che il thread deve mandare
	int numberOfTerminationMessages; 
	// puntatore alla coda condivisa
	nodePtr *ptr;
};

void* writeThread(void* arg){
	struct Args *funArg = (struct Args*)arg; 
	int numberOfMessages = (funArg->numberOfMessages);
	int numberOfTerminationMessages = (funArg->numberOfTerminationMessages);
	nodePtr *ptr = (funArg->ptr);
	
	// lo scrittore manda il numero di messaggi che gli sono
	// stati assegnati
	for(int i = 0; i < numberOfMessages; i++){
		int value = rand_r(&seed)%100;
		nodePtr newPtr = generateNode(value);
		lock(&myMtx);
		fprintf(stderr, "Writing value %d into queue\n", value);
		insertNode(ptr, newPtr);
		signal(&myCond);
		unlock(&myMtx);
	}
	// lo scrittore manda il numero di messaggi di terminazione
	// dei lettori che gli sono stati assegnati
	for(int i = 0; i < numberOfTerminationMessages; i++){
		nodePtr newPtr = generateNode(-1); // messaggio di terminazione per uno scrittore
		lock(&myMtx);
		insertNode(ptr, newPtr);
		signal(&myCond);
		unlock(&myMtx);
	}
	return 0;
}

void* readThread(void* arg){
	nodePtr *ptr = (nodePtr*)arg;
	int liveFlag = 1;
	
	// il lettore continua a ciclare finchè non
	// viene terminato esplicitamente leggendo
	// il valore -1 dalla coda
	while(liveFlag == 1){
		lock(&myMtx);
		while( *ptr == NULL ){
			wait(&myCond, &myMtx);
		}
		nodePtr removedNode = removeNode(ptr);
		if( removedNode->value == -1 ){ // devo terminare
			liveFlag = 0;
		}
		else{
			fprintf(stderr, "Reading value %d from queue\n", removedNode->value);
		}
		unlock(&myMtx);
		free(removedNode);
	}
	return 0;
}

void initializeWritersArgs(struct Args writersArgs[], nodePtr *queue, long numberOfWriters, long numberOfReaders, long numberOfMessages){
	// ripartisco tra gli scrittori i messaggi da mandare
	long mod1 = numberOfMessages%numberOfWriters;
	float div1 = ((float)numberOfMessages)/((float)numberOfWriters);
	for(size_t i = 0; i < mod1; i++){
		writersArgs[i].numberOfMessages = ceilf(div1);
	}
	for(size_t i = mod1; i < numberOfWriters; i++){
		writersArgs[i].numberOfMessages = floorf(div1);
	}
	// ripartisco tra gli scrittori i messaggi di terminazione
	// da mandare ai lettori
	long mod2 = numberOfReaders%numberOfWriters;
	float div2 = ((float)numberOfReaders)/((float)numberOfWriters);
	for(size_t i = 0; i < mod2; i++){
		writersArgs[i].numberOfTerminationMessages = ceilf(div2);
	}
	for(size_t i = mod2; i < numberOfWriters; i++){
		writersArgs[i].numberOfTerminationMessages = floorf(div2);
	}

	for(size_t i = 0; i < numberOfWriters; i++){
		writersArgs[i].ptr = queue;
	}
}

int main(int argc, char *argv[]){
	long numberOfWriters, numberOfReaders, numberOfMessages;
	nodePtr queue = NULL;
	seed = time(NULL);
	if( argc != 4 ){
		fprintf(stderr, "%s numero_scrittori numero_lettori numero_messaggi\n", argv[0]);
		return -1;
	}
	ce_ne0( isNumber(argv[1], &numberOfWriters), "Errore col numero di scrittori" );
	ce_ne0( isNumber(argv[2], &numberOfReaders), "Errore col numero di lettori" );
	ce_ne0( isNumber(argv[3], &numberOfMessages), "Errore col numero di messaggi" );
	if( numberOfWriters <= 0 || numberOfReaders <= 0 || numberOfMessages <= 0 ){
		fprintf(stderr, "Usare parametri positivi\n");
		return -1;
	}
	
	pthread_t writers[numberOfWriters], readers[numberOfReaders];
	struct Args writersArgs[numberOfWriters]; // argomenti dei thread scrittori
	
	initializeWritersArgs(writersArgs, &queue, numberOfWriters, numberOfReaders, numberOfMessages);
	// faccio partire gli scrittori
	for(size_t i = 0; i < numberOfWriters; i++){
		pthread_create(&writers[i], NULL, &(writeThread), (void*)(&writersArgs[i]));
	}
	// faccio partire i lettori
	for(size_t i = 0; i < numberOfReaders; i++){
		pthread_create(&readers[i], NULL, &(readThread), (void*)(&queue));
	}
	// faccio il join di tutti i thread
	for(size_t i = 0; i < numberOfWriters; i++){
		pthread_join(writers[i], NULL);
	}
	for(size_t i = 0; i < numberOfReaders; i++){
		pthread_join(readers[i], NULL);
	}
	return 0;
}
