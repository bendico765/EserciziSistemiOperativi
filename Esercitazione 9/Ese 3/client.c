#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include "utils.h"
#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define BUFFERSIZE 128
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

size_t strnlen(const char *s, size_t maxlen);

int initializeClientAndConnect(){
	int socket_fd;
	struct sockaddr_un sa;
	ce_less1(socket_fd = socket(AF_UNIX, SOCK_STREAM, 0), "Errore nella socket");
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family = AF_UNIX;
	while(connect(socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1){
		if( errno == ENOENT ){
			sleep(1);
		}
		else{
			return -1;
		}
	}
	return socket_fd;
}

int main(int argc, char *argv[]){
	int socket_fd;
	char buffer[BUFFERSIZE];
	if( argc != 2 ){
		printf("%s messaggio\n", argv[0]);
		return 0;
	}
	int size = strnlen(argv[1], BUFFERSIZE);
	ce_less1(socket_fd = initializeClientAndConnect(),"Errore nella connessione al socket");
	printf("Mi sono connesso al server!\n");
	printf("Mando: %s\n", argv[1]);
	// mando la dimensione della stringa
	ce_less1(writen(socket_fd, &size, sizeof(int)), "Errore nella write1");
	// mando la stringa effettiva
	ce_less1(writen(socket_fd, argv[1], size*sizeof(char)), "Errore nella write2");
	// leggo la dimensione della risposta
	ce_less1(readn(socket_fd, &size, sizeof(int)), "Errore nella read1");
	// leggo la risposta effettiva
	ce_less1(readn(socket_fd, buffer, size*sizeof(char)), "Errore nella read2");
	printf("Ho ricevuto: %s\n", buffer);
	ce_less1(close(socket_fd), "Errore nella close del socket");
	return 0;
}
