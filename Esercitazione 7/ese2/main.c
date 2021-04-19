/*
	N filosofi siedono ad una tavola con un piatto di spaghetti 
	davanti ed una forchetta alla loro destra ed una alla loro 
	sinistra. 
	Per mangiare gli spaghetti un filosofo ha bisogno di entrambe 
	le forchette vicine (!). Ogni filosofo è impegnato 
	ininterrottamente in una sequenza di 3 attivita': meditare, 
	cercare di acquisire le forchette e mangiare. Scrivere un 
	programma C che attivi N threads filosofi (con N>=5), ognuno 
	dei quali esegue il ciclo descritto in precedenza per 100 
	volte. 
	La meditazione e la fase in cui il filosofo mangia deve essere 
	implementata con un ritardo variabile usando ad esempio la 
	chiamata di sistema nanosleep e la funzione rand_r().
	
	SUGGERIMENTO: Per evitare il deadlock nell'aquisizione delle 
	forchette, una possibile soluzione è definire un ordinamento 
	opportuno per l'acquisizione delle forchette da parte di ogni 
	filosofo. 
	
	SOLUZIONE: I filosofi di indice pari prendono prima la
	bacchetta di indice i+1 e poi i, i filosofi di indice 
	dispari al contrario
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "utils.h"

int rand_r(unsigned int *seedp);
int nanosleep(const struct timespec *req, struct timespec *rem);

static unsigned int seed;

void lock(pthread_mutex_t *mtx){
	int err;
	if( ( err = pthread_mutex_lock(mtx)) != 0){
		errno = err;
		perror("Errore durante la lock");
		pthread_exit((void*)&errno);
	}
}

void unlock(pthread_mutex_t *mtx){
	int err;
	if( ( err = pthread_mutex_unlock(mtx)) != 0){
		errno = err;
		perror("Errore durante la unlock");
		pthread_exit((void*)&errno);
	}
}

struct filosofo{
	int id;
	pthread_mutex_t leftForkMtx;
	pthread_mutex_t rightForkMtx;
};

void mangia(){ 
	int value = rand_r(&seed); 
	struct timespec remaining, request = {0, value};
	nanosleep(&request, &remaining);
}

void medita(){ 
	int value = rand_r(&seed); 
	struct timespec remaining, request = {0, value};
	nanosleep(&request, &remaining);
}

void* threadFilosofo(void *arg){
	struct filosofo *fil = (struct filosofo*)arg;
	for(int i = 0; i < 3; i++){
		medita();
		if( fil->id % 2 == 0 ){ // caso pari
			lock( &(fil->rightForkMtx) );
			lock( &(fil->leftForkMtx) );
		}
		else{
			lock( &(fil->leftForkMtx) );
			lock( &(fil->rightForkMtx) );
		}
		mangia();
		unlock( &(fil->leftForkMtx) );
		unlock( &(fil->rightForkMtx) );
	}
	pthread_exit(0);
}

void initializeForks(pthread_mutex_t forkMtx[], long n){
	for(int i = 0; i < n; i++){ pthread_mutex_init(&forkMtx[i], NULL); }
}

int main(int argc, char *argv[]){
	long numFilosofi;
	if( inputControl(argc, argv, &numFilosofi) != 0 ){
		return -1;
	}
	seed = time(NULL);
	pthread_t thread_ids[numFilosofi];
	pthread_mutex_t forkMtx[numFilosofi];
	struct filosofo arrayFilosofi[numFilosofi];
	
	initializeForks(forkMtx, numFilosofi);
	for(int i = 0; i < numFilosofi; i++){
		//inizializzazione struct filosofo
		arrayFilosofi[i].leftForkMtx = forkMtx[i];
		arrayFilosofi[i].rightForkMtx = forkMtx[(i+1)%numFilosofi];
		pthread_create(&thread_ids[i], NULL, &threadFilosofo, &(arrayFilosofi[i]) );
	}
	
	for(int i = 0; i < numFilosofi; i++){
		pthread_join(thread_ids[i], NULL);
	}
	return 0;
}
