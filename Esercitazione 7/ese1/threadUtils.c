#include <errno.h>
#include <pthread.h>
#include <stdio.h>

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

void wait(pthread_cond_t *cond, pthread_mutex_t *mtx){
	int err;
	if( (err = pthread_cond_wait(cond, mtx)) != 0 ){
		errno = err;
		perror("Errore durante la wait");
		pthread_exit((void*)&err);
	}
}

void signal(pthread_cond_t *cond){
	int err;
	if( (err = pthread_cond_signal(cond)) != 0 ){
		errno = err;
		perror("Errore durante la signal");
		pthread_exit((void*)&err);
	}
}
