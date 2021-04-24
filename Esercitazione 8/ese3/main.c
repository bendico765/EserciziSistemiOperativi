/*
Realizzare una semplice calcolatrice scientifica testuale. 
Il programma legge da tastiera una operazione alla volta, 
una per riga (es. sqrt(3) + 2/5), quindi per ogni richiesta 
forka ed esegue la calcolatrice testuale fornita dal 
sistema bc (tipicamente installato in /usr/bin – utilizzare i
parametri -lq, cioè bc -lq) a cui, tramite pipe senza nome, 
passa l'operazione da svolgere e prende il risultato calcolato
(ridirigere nella pipe sia lo standard output che lo standard error
di 'bc'). Il risultato ottenuto verrà stampato sullo standard
output secondo il seguente formato:
Operazione: op
Risultato : res 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#define MAX_SIZE 512
#define ce_less1(var,msg) if(var == -1){perror(msg);exit(-1);}

int main(){
	char buffer[MAX_SIZE];
	printf("Operazione: ");
	while( fgets(buffer, MAX_SIZE, stdin) != NULL && strncmp(buffer, "quit\n", strlen(buffer)) != 0 ){
		int pipeChannel1[2]; // pipe bc-main
		int pipeChannel2[2]; // pipe main-bc
		int pid;
		errno = 0;
		ce_less1(pipe(pipeChannel1), "Errore nella pipe");
		ce_less1(pipe(pipeChannel2), "Errore nella pipe");
		ce_less1((pid = fork()), "Errore nella fork");
		if( pid == 0 ){ // processo figlio: bc -lq
			// rendirizzo lo standard input
			ce_less1(dup2(pipeChannel1[0], 0), "Errore nella dup2");
			close(pipeChannel1[1]);
			close(pipeChannel1[0]);
			// rendirizzo lo standard output ed error
			ce_less1(dup2(pipeChannel2[1], 1), "Errore nella dup2");
			ce_less1(dup2(pipeChannel2[1], 2), "Errore nella dup2");
			close(pipeChannel2[1]);
			close(pipeChannel2[0]);
			// sono pronto a fare la exe*
			const char *file = "/usr/bin/bc";
			char *const argv[3] = {"/usr/bin/bc", "-lq", NULL};
			execvp(file, argv);
			perror("Errore nella execvp");
			exit(-1);
		}
		else{ // padre
			close(pipeChannel1[0]);
			close(pipeChannel2[1]);
		}
		// scrivo l'operazione sullo stdin di bc
		ce_less1(write(pipeChannel1[1], buffer, strlen(buffer)), "Errore durante la write");
		close(pipeChannel1[1]);
		// leggo il risultato dallo stdout di bc
		int readChars;
		ce_less1( (readChars = read(pipeChannel2[0], buffer, MAX_SIZE)) , "Errore durante la read");
		close(pipeChannel2[0]);
		// bc non mette il carattere terminatore al risultato
		buffer[readChars] = '\0';
		printf("Risultato: %s", buffer);
		wait(NULL); // aspetto l'esito del calcolo
		printf("Operazione: ");
	}
	printf("Bye bye!\n");
	return 0;
}
