/*
	Modificare l'Esercizio 2 in modo da gestire i segnali 
	nel server in modo sincrono utilizzando un thread dedicato
	(non devono essere installati signal-handlers – 
	se non per ignorare SIGPIPE) e la chiamata di libreria
	sigwait. 
	Fare attenzione a bloccare tutti i segnali che si vogliono
	gestire in tutti i threads al fine di evitare il delivery
	“accidentale” ad un thread diverso dal thread gestore dei
	segnali.
	
	TIP: Inviare segnali manualmente

	Usare ps aux per vedere la lista dei processi con relativo PID
	Usare kill -L per vedere la lista dei segnali con relativo id
	Usare kill -segnale pid per inviare il segnale “segnale” 
	al processo con PID “pid.
	
	Ad esempio: kill -9 42 per inviare 9 (SIGKILL) 
	al processo con PID 42 (Termina il processo. 
	usare con cautela!)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#include "utils.h"
#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define BUFFERSIZE 64
#define MAX_REQUESTS 5
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

int termination_flag = 0;
int signal_handler_pipe[2]; // main - signal handler

int initializeServer(){
	int fd;
	struct sockaddr_un sa;
	ce_less1( fd = socket(AF_UNIX, SOCK_STREAM, 0) , "Errore nella creazione della socket");
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family = AF_UNIX;
	ce_less1( bind(fd, (struct sockaddr *)&sa, sizeof(sa)) ,"Errore nella bind");
	ce_less1(listen(fd, SOMAXCONN), "Errore nella listen");
	return fd;
}

// THREAD SIGNAL HANDLER PERSONALIZZATO
void* threadSignalHandler(void *arg){
	sigset_t *set = (sigset_t*)arg;
	int signum;
	
	// il thread rimane in attesa di segnali
	while( termination_flag == 0 ){
		int ret = sigwait(set, &signum);
		if( ret != 0 ){
			errno = ret;
			perror("Errore sigwait");
			return NULL;
		}
		fprintf(stderr, "THREAD: ho appena ricevuto %d\n", signum);
		// gestisco il segnale ricevuto
		switch( signum ){
			case SIGINT:
			case SIGTERM:
			case SIGQUIT:
			case SIGHUP:
				termination_flag = 1;
				close(signal_handler_pipe[1]);
				break;
			default:
				break;
		}
	}
	return NULL;
}

void signalHandling(){
	sigset_t set;
	// maschero SIGINT. SIGQUIT, SIGTERM e SIGHUP
	ce_less1(sigemptyset(&set), "Errore sigemptyset");
	ce_less1(sigaddset(&set, SIGINT), "Errore sigaddset");
	ce_less1(sigaddset(&set, SIGQUIT), "Errore sigaddset");
	ce_less1(sigaddset(&set, SIGTERM), "Errore sigaddset");
	ce_less1(sigaddset(&set, SIGHUP), "Errore sigaddset");
	if( pthread_sigmask(SIG_BLOCK, &set, NULL) != 0 ){
		fprintf(stderr, "Errore pthread_sigmask\n");
		exit(-1);
	}
	// ignoro SIGPIPE
	struct sigaction s;
	memset((void*)&s, 0, sizeof(struct sigaction));
	s.sa_handler = SIG_IGN;
	ce_less1(sigaction(SIGPIPE, &s, NULL), "Errore sigaction");

	// lancio il thread per il signal handling
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, &threadSignalHandler, (void*)&set);
	pthread_detach(thread_id);
}

int main(int argc, char *argv[]){
	signalHandling();
	int socket_fd = initializeServer();
	int fd_num = 0; // max fd attivo
	fd_set set; // insieme fd attivi
	fd_set rdset; // insieme fd attesi in lettura
	
	
	// inizializzo la pipe tra main e thread signal handler
	ce_less1(pipe(signal_handler_pipe), "Errore pipe");
	
	// mantengo il massimo indice di descrittore
	// attivo in fd_num
	if( socket_fd > fd_num ){
		fd_num = socket_fd;
	}
	if( signal_handler_pipe[0] > fd_num ){
		fd_num = signal_handler_pipe[0];
	}
	FD_ZERO(&set);
	FD_SET(socket_fd, &set);
	FD_SET(signal_handler_pipe[0], &set);
	
	while(termination_flag == 0){
		// bisogna inizializzare ogni volta rdset 
		// perchè set lo modifica
		rdset = set; // preparo maschera per select
		if( select(fd_num+1, &rdset, NULL, NULL, NULL) == -1 && errno != EINTR){
			perror("Errore nella select");
			exit(-1);
		}
		if( termination_flag == 0 ){
			for(int fd = 0; fd <= fd_num; fd++ ){
				int fd_connect;
				if( FD_ISSET(fd, &rdset) ){
					if( fd == socket_fd ){ // sock connect pronto
						fd_connect = accept(socket_fd, NULL, 0);
						fprintf(stderr, "Ho accettato una connessione\n");
						FD_SET(fd_connect, &set);
						if( fd_connect > fd_num ){
							fd_num = fd_connect;
						}
					}
					else{	// sock I/O pronto
						int size_message;
						int readChars;
						// un ritardo artificiale per testare il server
						sleep(2);
						// leggo il numero di caratteri da ricevere
						ce_less1( readChars = readn(fd, &size_message, sizeof(int)), "Errore nella read1");
						if( readChars == 0 ){ // EOF client finito
							FD_CLR(fd, &set);
							if( fd == fd_num ){
								fd_num -= 1;
							}
							close(fd);
						}
						else{
							// ricevo il messaggio effettivo
							char *buffer = calloc(size_message, sizeof(char));
							if( buffer == NULL ){
								perror("Errore nella calloc");
								exit(-1);
							}
							ce_less1( readChars = readn(fd, buffer, size_message*sizeof(char)), "Errore nella read2");
							for(int i = 0; i < size_message; i++){
								buffer[i] = toupper(buffer[i]);
							}
							ce_less1( write(fd, &size_message, sizeof(int)), "Errore nella write1" );
							ce_less1( write(fd, buffer, size_message*sizeof(char)), "Errore nella write2");
							free(buffer);
						}
					}
				}
			}
		}
	}
	// chiudo la socket e ne elimino il file 
	close(socket_fd);
	ce_less1(unlink(SOCKNAME), "Errore nella unlink");
	return 0;
}
