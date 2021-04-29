#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define BUFFERSIZE 512
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

int initializeClientAndConnect(){ // funzione che inizializza il fd del socket lato client
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

int main(){
	int socket_fd;
	char buffer[BUFFERSIZE];
	// provo la connessione al server
	ce_less1(socket_fd = initializeClientAndConnect(),"Errore nella connessione al socket");
	printf("Mi sono connesso al server!\n");
	// ciclo finchè l'utente non inserisce "quit" oppure
	// finchè non c'è un buffer overflow (ricordarsi di gestirlo)
	int readChars;
	printf("Operazione: ");
	while( fgets(buffer, BUFFERSIZE, stdin) != NULL && strncmp(buffer, "quit\n", strlen(buffer)) != 0 ){
		ce_less1(write(socket_fd, buffer, strlen(buffer)), "Errore nella write");
		ce_less1(readChars = read(socket_fd, buffer, BUFFERSIZE), "Errore nella read");
		buffer[readChars] = '\0';
		printf("Ho ricevuto: %s\n", buffer);
		printf("Operazione: ");
	}
	close(socket_fd);
	return 0;
}
