#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define ce_less1(var, msg) if( (var) == -1 ){perror(msg); exit(-1);}

int main(int argc, char *argv[]){
	if( argc != 2 ){
		fprintf(stderr, "%s number\n", argv[0]);
		return -1;
	}
	int pipe1[2]; // dec1-dec2
	int pipe2[2]; // dec2-dec1
	errno = 0;
	ce_less1(pipe(pipe1), "Errore nella creazione della prima pipe" );
	ce_less1(pipe(pipe2), "Errore nella creazione della seconda pipe");
	int pid1, pid2;
	ce_less1( pid1 = fork(), "Errore nella fork");
	if( pid1 == 0 ){ // dec1
		// chiudo le pipe che non userò
		close(pipe1[1]);
		close(pipe2[0]);
		// redirigo stdout ed stdin nelle pipe
		ce_less1( dup2(pipe1[0], 0), "Errore nella dup2");
		ce_less1( dup2(pipe2[1], 1), "Errore nella dup2");
		close(pipe1[0]);
		close(pipe2[1]);
		// faccio la exec
		execl("dec", "dec", argv[1], NULL); // gli passo il parametro intero
		perror("Qualcosa è andato storto nella exec di dec1");
		exit(-1);
	}
	ce_less1( pid2 = fork(), "Errore nella fork");
	if( pid2 == 0 ){ // dec2
		// chiudo le pipe che non userò
		close(pipe1[0]);
		close(pipe2[1]);
		// redirigo stdout ed stdin nelle pipe
		ce_less1( dup2(pipe1[1], 1), "Errore nella dup2");
		ce_less1( dup2(pipe2[0], 0), "Errore nella dup2");
		close(pipe1[1]);
		close(pipe2[0]);
		// faccio la exec
		execl("dec", "dec", NULL); // non gli passo l'intero; è il ricevente
		perror("Qualcosa è andato storto nella exec di dec2");
		exit(-1);
	}
	// chiudo tutte le pipe che tanto non servono
	close(pipe1[0]);
	close(pipe1[1]);
	close(pipe2[0]);
	close(pipe2[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	return 0;
}
