/*
	Modificare l'Esercizio 3 dell'Esercitazione 9 in modo da
	gestire la terminazione del server a seguito della 
	ricezione di un segnale di terminazione (SIGINT, SIGQUIT,
	SIGTERM, SIGHUP) con l'obiettivo di lasciare il sistema 
	in uno stato consistente, cioè eliminare dal file system 
	il socket AF_UNIX creato per accettare le connessioni, 
	eventuali file temporanei e liberare la memoria allocata 
	dinamicamente al fine di poter controllare eventuali memory
	leaks con valgrind. 
	Il segnale SIGPIPE deve essere invece ignorato. 
	La gestione dei segnali deve essere effettuata installando 
	un signal-handler con la SC sigaction. 
	Il signal-handler deve accedere solamente a variabili 
	dichiarate volatile sig_atomic_t e deve usare solo 
	chiamate asynchronous-signal-safe (vedere anche man 7 
	signal-safety).

	Testare l'esecuzione del server lanciandolo con il comando 
	valgrind –leak-check=full e verficare che non ci siano 
	messaggi di errore all'uscita dopo aver inviato un segnale 
	di terminazione al termine del test. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
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

volatile sig_atomic_t termination_flag = 0;

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

// SIGNAL HANDLER PERSONALIZZATO
void signalHandler(int signum){
	write(2, "Ho ricevuto un segnale\n", 23);
	termination_flag = 1;
}

void signalHandling(){
	// maschero tutti  i segnali finchè i 
	// gestori permanenti non sono installati
	sigset_t set;
	ce_less1( sigfillset(&set), "Errore sigfillset");
	ce_less1( pthread_sigmask(SIG_SETMASK, &set, NULL), "Errore sigmask" );
	// posso iniziare la gestione segnali
	struct sigaction s;
	memset((void*)&s, 0, sizeof(struct sigaction));
	// ignoriamo sigpipe
	s.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &s, NULL);
	// impostiamo il signal handler per 
	// SIGINT, SIGQUIT, SIGTERM, SIGHUP
	s.sa_handler = signalHandler;
	s.sa_flags = SA_RESTART;
	ce_less1(sigaction(SIGINT, &s, NULL), "Errore sigaction SIGINT");
	ce_less1(sigaction(SIGQUIT, &s, NULL), "Errore sigaction SIGQUIT");
	ce_less1(sigaction(SIGTERM, &s, NULL), "Errore sigaction SIGTERM");
	ce_less1(sigaction(SIGHUP, &s, NULL), "Errore sigaction SIGHUP");
	// tolgo la maschera
	ce_less1( sigemptyset(&set), "Errore sigemptyset" );
	ce_less1( pthread_sigmask(SIG_SETMASK, &set, NULL), "Errore sigmask" );
}

int main(int argc, char *argv[]){
	signalHandling();
	int socket_fd = initializeServer();
	int fd_num = 0; // max fd attivo
	fd_set set; // insieme fd attivi
	fd_set rdset; // insieme fd attesi in lettura
	
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
		if( select(fd_num+1, &rdset, NULL, NULL, NULL) == -1 && errno != EINTR){
			perror("Errore nella select");
			exit(-1);
		}
		if( termination_flag == 1 ){
			break;
		}
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
	// chiudo la socket e ne elimino il file 
	close(socket_fd);
	ce_less1(unlink(SOCKNAME), "Errore nella unlink");
	return 0;
}
