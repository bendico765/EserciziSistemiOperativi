/*
Realizzare una shell rudimentale (dummyshell) che legge un comando con
eventuali parametri dallo standard input e ne invoca l'esecuzione utilizzando
una funzione di libreria della famiglia exec*. La shell deve terminare se
viene digitato il comando 'exit'. Il formato dei comandi accettati dalla shell
e' molto semplice e non non prevede metacaratteri, redirezione, pipe,
lettura di variabili d'ambiente, etc…
*/

#define BUFFERSIZE 256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int getNumberOfSpaces(char *string, int len){
  int spaces = 0;
  for(int i = 0; i < len; i++){
    if( isspace(string[i]) != 0 ){
      spaces++;
    }
  }
  return spaces;
}

int cmdRead(char ***myArgv){
  char buffer[BUFFERSIZE];
  int len;
  if( fgets(buffer, BUFFERSIZE, stdin) == NULL ){
    return -1;
  }
  else{
    len = strlen(buffer);
    if( len == 1 && buffer[0] == '\n' ){
      return -1;
    }
    buffer[len-1] = '\0'; // elimino il newline
    len--;
    int numberOfArguments = getNumberOfSpaces(buffer, len)+2;

    errno = 0;
    *myArgv = malloc(sizeof(char*)*numberOfArguments);
    (*myArgv)[numberOfArguments-1] = NULL;
    if( *myArgv == NULL ){
      perror("Errore durante la malloc");
      return -1;
    }
    else{
      char *saveptr = NULL;
      (*myArgv)[0] = strtok_r(buffer, " ", &saveptr);
      for(int i = 1; i < numberOfArguments; i++){
        (*myArgv)[i] = strtok_r(NULL, " ", &saveptr);
      }
      return 0;
    }
  }
}

int main(){
    printf("Shell avviata\n");
    fflush(stdin);
    int exit_flag = 0;
    while(exit_flag == 0){
      char **myArgv = NULL;
      printf(">> ");
      if( cmdRead(&myArgv) == 0 ){
        if( strcmp(myArgv[0], "exit") == 0){
          exit_flag = 1;
        }
        else{
          errno = 0;
          pid_t pid = fork();
          if( pid == -1 ){
            perror("Errore durante la fork");
          }
          else{
            if( pid == 0 ){ // processo figlio
              execvp(myArgv[0],(char * const*)myArgv);
              perror("Errore nella exec!");
              exit(-1);
            }
            else{
              waitpid(pid, NULL, 0);
            }
          }
        }
        free(myArgv);
      }
      else{
        printf("Error\n");
      }
    }
    printf("Chiusura della shell...\n");
    return 0;
}
