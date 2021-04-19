#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
/*
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve effettuare fork di processi con il vincolo che
 * ogni processo esegua 'doFib' al più una volta.
 * Se l'argomento doPrint e' 1 allora la funzione stampa il numero calcolato
 * prima di passarlo al processo padre.
 */
static void doFib(int n, int doPrint){
  if( n <= 0 ){
    printf("Restituisco 0\n");
    exit(0);
  }
  if( n == 1 ){
    printf("Restituisco 1\n");
    exit(1);
  }
  n--;
  pid_t pid1 = fork();
  if( pid1 == -1 ){
    fprintf(stderr, "Errore nella fork 1!\n");
    exit(-1);
  }
  if( pid1 == 0 ){ // processo figlio1
    doFib(n, doPrint);
  }
  else{
    n--;
    pid_t pid2 = fork();
    if( pid2 == -1 ){
      fprintf(stderr, "Errore nella fork 2!\n");
      exit(-1);
    }
    if( pid2 == 0 ){ // processo figlio2
      doFib(n, doPrint);
    }
    else{
      int wstatus1, wstatus2;
      int n1, n2;
      waitpid(pid1, &wstatus1, 0);
      waitpid(pid2, &wstatus2, 0);
      if( WIFEXITED(wstatus1) != 0 ){
        n1 = WEXITSTATUS(wstatus1);
      }
      if( WIFEXITED(wstatus2) != 0 ){
        n2 = WEXITSTATUS(wstatus2);
      }
      printf("Restituisco %d\n", n1+n2);
      exit(n1+n2);
    }
  }
}

int main(int argc, char *argv[]) {
    // questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.
    const int NMAX=13;
    int arg;

    if(argc != 2){
	fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	return EXIT_FAILURE;
    }
    arg = atoi(argv[1]);
    if(arg <= 0 || arg > NMAX){
    	fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
    	return EXIT_FAILURE;
    }
    doFib(arg, 1);
    return 0;
}
