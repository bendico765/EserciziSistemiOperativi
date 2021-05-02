/*
	Realizzare un programma C che implementa un server 
	che rimane sempre attivo in attesa di richieste da 
	parte di uno o piu' processi client su una socket 
	di tipo AF_UNIX. 
	Ogni client richiede al server la trasformazione di 
	tutti i caratteri di una stringa da minuscoli a maiuscoli 
	(es. ciao –> CIAO). 

	Realizzare il server secondo il modello single-threaded. 
	Il server fa uso della chiamata di sistema select (o poll) 
	per gestire sia nuove connessioni da parte di nuovi 
	client che la gestione dei messaggi inviati dai 
	client gia' connessi. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define BUFFERSIZE 64
#define MAX_REQUESTS 5
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

int main(int argc, char *argv[]){
	int socket_fd = initializeServer();
	int fd_num = 0; // max fd attivo
	fd_set set; // insieme fd attivi
	fd_set rdset; // insieme fd attesi in lettura
	char buffer[BUFFERSIZE];
	
	// mantengo il massimo indice di descrittore
	// attivo in fd_num
	if( socket_fd > fd_num ){
		fd_num = socket_fd;
	}
	FD_ZERO(&set);
	FD_SET(socket_fd, &set);
	while(1){
		// bisogna inizializzare ogni volta rdset 
		// perchè set lo modifica
		rdset = set; // preparo maschera per select
		ce_less1( select(fd_num+1, &rdset, NULL, NULL, NULL), "Errore nella select");
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
					int readChars;
					ce_less1( readChars = read(fd, buffer, BUFFERSIZE), "Errore nella read");
					if( readChars == 0 ){ // EOF client finito
						FD_CLR(fd, &set);
						if( fd == fd_num ){
							fd_num -= 1;
						}
						close(fd);
					}
					else{
						for(int i = 0; i < readChars; i++){
							buffer[i] = toupper(buffer[i]);
						}
						ce_less1( write(fd, buffer, strlen(buffer)), "Errore nella write");
					}
				}
			}
		}
		
	}
	// chiudo la socket e ne elimino il file 
	close(socket_fd);
	unlink(SOCKNAME);
	return 0;
}
