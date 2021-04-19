/*
nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h

Estendere l'esercizio 1 associando ad ogni opzione del main una funzione
che ne faccia la gestione e la stampa sullo standard output.
Ad esempio, la gestione dell'opzione '-n' viene fatta dalla
funzione 'arg_n(const char*)' la quale convertirà l'argomento associato
all'opzione ad intero e farà la stampa sullo standard output,
ritornado -1 in caso di errore e 0 in caso di successo.
Memorizzare tali funzioni in un vettore con nome 'V'.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define N_ARGUMENTS 4

/*
  La funzione riceve una stringa s in ingresso e prova a convertirla in long,
  restituendo un codice per segnalare l'esito dell'operazione

  Restituisce:
  0: ok
  1: non e' un numbero
  2: overflow/underflow
*/
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

int arg_n(const char* arg){
  long val;
  int res = isNumber(arg, &val);
  if( res == 0 ){
    printf("L'argomento del parametro -n è: %d\n", (int)val);
    return 0;
  }
  return -1;
}
int arg_m(const char* arg){
  long val;
  int res = isNumber(arg, &val);
  if( res == 0 ){
    printf("L'argomento del parametro -m è: %d\n", (int)val);
    return 0;
  }
  return -1;
}
int arg_o(const char* arg){
  printf("L'argomento del parametro -o è: %s\n", arg);
  return 0;
}
int arg_h(const char* arg){
  printf("%s\n", arg);
  return 0;
}
int main(int argc, char* argv[]) {
  int (*V[N_ARGUMENTS])(const char*) = {arg_h, arg_m, arg_n, arg_o};
  int opt;
  while ((opt = getopt(argc,argv, "n:m:o:h")) != -1) {
    switch(opt) {
    case '?':
       printf("Parametro sconosciuto\n");
       break;
    default:
      // invocazione della funzione di gestione passando come parametro l'argomento restituito da getopt
      if (V[opt%4]( (optarg==NULL ? argv[0] : optarg) ) == -1) {
        printf("Errore con il seguente parametro: %c\n", opt);
      }
      break;
    }
  }
  return 0;
}
