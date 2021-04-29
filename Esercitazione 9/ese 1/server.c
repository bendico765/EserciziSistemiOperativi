/*
	 Realizzare in C un programma client ed un programma server. 
	 I due processi intergiscono utilizzando socket AF_UNIX. 
	 Il client apre una connessione verso il server ed 
	 invia richieste (sotto forma di stringhe) corrispondenti 
	 ad operazioni aritmetiche inserite dall'utente 
	 (es. 2+1, 3/4 + 2/7, sqrt(5) + 3.14, …). 
	 Il server gestisce una connessione alla volta verso un client. 
	 Il client, dopo l'invio della stringa contenente 
	 l'operazione attende il risultato dal server prima di 
	 inviare una nuova richiesta. Per eseguire l'operazione, 
	 il server lancia un processo che esegue la calcolatrice 
	 testuale 'bc'. 
	 Il messaggio di risposta del server è una stringa contenente 
	 il risultato calcolato. 
	 La sequenza di operazioni termina quando il client riceve 
	 in input la stringa “quit” dall'utente che fa chiudere la 
	 connessione con il server. 
	 Il server si rimette in attesa di ricevere una nuova 
	 connessione. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFERSIZE 512
#define SOCKNAME "./mysock"
#define MAXCONN 1
#define MAX_REQUESTS 1
#define UNIX_PATH_MAX 108
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

int initializeServer(){
	int fd;
	struct sockaddr_un sa;
	ce_less1( fd = socket(AF_UNIX, SOCK_STREAM, 0) , "Errore nella creazione della socket");
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family = AF_UNIX;
	ce_less1( bind(fd, (struct sockaddr *)&sa, sizeof(sa)) ,"Errore nella bind");
	ce_less1( listen(fd, MAXCONN) ,"Errore nella bind");
	return fd;
}

int main(){
	int socket_fd, pid, request_counter;
	int pipe1[2]; // server-calcolatore
	int pipe2[2]; // calcolatore-server
	char buffer[BUFFERSIZE];
	// inizializzazione pipe
	ce_less1(pipe(pipe1), "Errore nella pipe");
	ce_less1(pipe(pipe2), "Errore nella pipe");
	// faccio la fork
	ce_less1( pid = fork(), "Errore nella fork");
	if( pid == 0 ){ // processo calcolatore
		// reindirizzo stdin, stdout ed stderr
		ce_less1(dup2(pipe2[0], 0), "Errore nella dup2");
		ce_less1(dup2(pipe1[1], 1), "Errore nella dup2");
		ce_less1(dup2(pipe1[1], 2), "Errore nella dup2");
		// chiudo le pipe
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		const char *file = "/usr/bin/bc";
		char *const argv[3] = {"/usr/bin/bc", "-lq", NULL};
		execvp(file, argv);
		perror("Errore nella exec");
		exit(-1);
	}
	// il server chiude i descrittori che non gli servono
	close(pipe1[1]);
	close(pipe2[0]);
	// inizializzazione server
	socket_fd = initializeServer();
	request_counter = 0;
	while( request_counter < MAX_REQUESTS ){
		// il server accetta le connessioni
		int connection_fd = accept(socket_fd, NULL, 0);
		printf("Ho accettato una connessione!\n");
		fflush(stdout);
		int readChars;
		while( (readChars = read(connection_fd, buffer, BUFFERSIZE)) > 0 && strncmp(buffer, "quit\n", strlen(buffer)) != 0 ){
			// scrivo l'operazione sullo stdin di bc
			ce_less1(write(pipe2[1], buffer, strlen(buffer)), "Errore durante la write");
			// leggo il risultato dallo stdout di bc
			ce_less1( (readChars = read(pipe1[0], buffer, BUFFERSIZE)) , "Errore durante la read");
			// bc non mette il carattere terminatore al risultato
			buffer[readChars] = '\0';
			// mando il risultato al client
			write(connection_fd, buffer, strlen(buffer));
		}	
		close(connection_fd);
		request_counter += 1;
	}
	ce_less1(write(pipe2[1], "quit", 4), "Errore durante la write");
	close(pipe1[0]);
	close(pipe2[1]);
	close(socket_fd);
	unlink(SOCKNAME);
	return 0;
}
