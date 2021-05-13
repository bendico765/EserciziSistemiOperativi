/*
	Scrivere un programma C che conta il numero di volte in cui
	l'utente invia il segnale SIGINT (Ctl-C) al processo. 
	
	Quando il processo riceve il segnale SIGTSTP (Ctl-Z), 
	il numero di SIGINT ricevuti viene stampato sullo standard
	output. 
	
	Al terzo segnale SIGTSTP, il processo chiede all'utente se 
	deve terminare o no attendendo una risposta per 10 secondi
	dallo standard input. 
	
	Se l'utente non risponde entro 10 secondi un segnale 
	SIGALRM termina il processo. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#define ce_neg1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

volatile sig_atomic_t sigint_counter = 0;
volatile sig_atomic_t sigtstp_counter = 0;
volatile sig_atomic_t termination_request_flag = 0;

void sigintHandler(int signum){
	sigint_counter++;
}

void sigtstpHandler(int signum){
	char buffer[128];
	int bufferChars;
	bufferChars = sprintf(buffer, "\nHo ricevuto %d sigint\n", sigint_counter);
	// CERCARE MODO SIGNAL SAFE
	write(1, buffer, bufferChars);
	sigtstp_counter++;
	if( sigtstp_counter == 3 ){
		termination_request_flag = 1;
	}
}

void sigalrmHandler(int signum){
	write(1, "\nHandler alarm: termino il processo\n", 36);
	_exit(0);
}

void handleSignals(){
	struct sigaction sig_struct;
	// gestione SIGINT
	memset((void*)&sig_struct, 0, sizeof(sig_struct));
	sig_struct.sa_handler = sigintHandler;
	ce_neg1(sigaction(SIGINT, &sig_struct, NULL), "Errore sigaction SIGINT");
	// gestione SIGTSTP
	memset((void*)&sig_struct, 0, sizeof(sig_struct));
	sig_struct.sa_handler = sigtstpHandler;
	ce_neg1(sigaction(SIGTSTP, &sig_struct, NULL), "Errore sigaction SIGTSTP");
	// gestione SIGALRM
	memset((void*)&sig_struct, 0, sizeof(sig_struct));
	sig_struct.sa_handler = sigalrmHandler;
	ce_neg1(sigaction(SIGALRM, &sig_struct, NULL), "Errore sigaction SIGALRM");
}
int main(){
	int exit_flag = 0;
	handleSignals();
	while( exit_flag == 0 ){
		pause();
		if( termination_request_flag == 1 ){
			char buffer[1];
			termination_request_flag = 0;
			sigtstp_counter = 0;
			alarm(10);
			fprintf(stderr, "Mi blocco sulla read\n");
			read(0, buffer, 1);
			if( buffer[0] == 'y' ){
				exit_flag = 1;
			}
		}
	}
	return 0;
}
