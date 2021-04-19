/*
Scrivere un programma C che prende come argomento un intero N>1.
L'intero N rappresenta il numero di processi figli che devono essere
creati in totale. I processi vanno creati come segue: il processo main stampa
il suo pid e crea un figlio attendendone quindi la sua terminazione, il
processo figlio a sua volta stampa il suo pid e crea un altro figlio
attendendone la terminazione e cosi' via fino ad ottenere N processi figli
in tutto.
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int isNumber(const char* s, long* n) {
  if (s==NULL) return 1;
  if (strlen(s)==0) return 1;
  char* e = NULL;
  errno = 0;
  long val = strtol(s, &e, 10);
  if (errno == ERANGE) return 2; // overflow
  if (e != NULL && *e == (char)0) {
    *n = val;
    return 0;   // successo
  }
  return 1;   // non e' un numero
}

int main(int argc, char *argv[]){
  if( argc != 2 ){
    printf("./family number\n");
    return 0;
  }
  long n;
  if( isNumber(argv[1], &n) != 0 ){
    perror("Errore di conversione del parametro numerico\n");
    exit(-1);
  }
  if( n <= 1 ){
    printf("Il numero di figli deve essere maggiore o uguale a 1\n");
    return 0;}
  while(n >= 0){
    if( n == 0){
      printf("%d: sono l'ultimo discendente\n", (int)getpid());
    }
    else{
      for(int i = 0; i < n; i++){ printf("-");}
      printf(" %d: creo un processo figlio\n", (int)getpid());
      fflush(stdout);
      pid_t pid = fork();
      if( pid == -1 ){
        perror("Errore durante la fork\n");
        exit(-1);
      }
      if( pid != 0){ // processo padre
        waitpid(pid, NULL, 0);
        for(int i = 0; i < n; i++){ printf("-");}
        printf(" %d: terminato con successo\n", (int)getpid());
        return 0;
      }
    }
    n--;
  }
  printf("%d: terminato con successo\n", (int)getpid());
  return 0;
}
