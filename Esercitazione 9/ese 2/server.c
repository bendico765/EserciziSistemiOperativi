/*
	Realizzare un programma C che implementa un server 
	che rimane sempre attivo in attesa di richieste da 
	parte di uno o piu' processi client su una socket 
	di tipo AF_UNIX. 
	Ogni client richiede al server la trasformazione di 
	tutti i caratteri di una stringa da minuscoli a maiuscoli 
	(es. ciao –> CIAO). 
	Per ogni nuova connessione il server lancia un thread 
	POSIX che gestisce tutte le richieste del client 
	(modello “un thread per connessione”) e quindi termina 
	la sua esecuzione quando il client chiude la connessione.
	
	Per testare il programma implementare uno script bash 
	che lancia N>10 clients ognuno dei quali invia una o 
	piu' richieste al server multithreaded.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define BUFFERSIZE 64
#define MAX_REQUESTS 10
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

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

void* requestHandlerThread(void *arg){
	int fd = *((int*)arg);
	int readChars;
	char buffer[BUFFERSIZE];
	// ricevo la stringa dal client
	if( (readChars = read(fd, buffer, BUFFERSIZE) )== -1 ){
		perror("Errore nella read");
		close(fd);
		return 0;
	}
	sleep(1);
	// converto la stringa
	for(int i = 0; i < readChars; i++){
		buffer[i] = toupper(buffer[i]);
	}
	// la rimando al client
	if( write(fd, buffer, readChars) == -1 ){
		perror("Errore nella write");
		close(fd);
		return 0;
	}
	close(fd);
	return 0;
}

int main(int argc, char *argv[]){
	int socket_fd = initializeServer();
	int connections_counter = 0;
	while(connections_counter < MAX_REQUESTS){
		int connection_fd = accept(socket_fd, NULL, 0);
		printf("Ho accettato una connessione!\n");
		pthread_t thread;
		pthread_create(&thread, NULL, &(requestHandlerThread), (void*)&connection_fd);
		if( pthread_detach(thread) != 0){
			fprintf(stderr, "Errore durante il detach del thread\n");
		}
		connections_counter += 1;
	}
	// chiudo la socket e ne elimino il file 
	close(socket_fd);
	unlink(SOCKNAME);
	return 0;
}
