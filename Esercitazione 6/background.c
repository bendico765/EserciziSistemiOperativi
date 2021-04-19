/*
Scrivere un programma C che appena attivato va immediatamente in background,
attende per X secondi (eseguendo il programma /bin/sleep con una chiamata ad
una exec*) dove X e' l'argomento del programma e poi stampa il suo pid,
il pid del padre e quindi termina.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
  if( argc != 2 ){
    printf("./sleep num_secondi\n");
    return 0;
  }
  pid_t pid;
  pid = fork();
  if( pid == -1 ){
    perror("Errore durante la fork!\n");
    exit(-1);
  }
  if( pid == 0 ){ //figlio
    //eseguo la sleep
    execl("/bin/sleep", "sleep",argv[1], (char*)NULL);
    perror("Qualcosa è andato storto con la exec\n");
    exit(-1);
  }
  else{ // padre
  	waitpid(pid, NULL, 0);
    printf("Child's PID: %d\n", (int)getpid());
    printf("Parent's PID: %d\n", (int)getppid());
  }
  return 0;
}
