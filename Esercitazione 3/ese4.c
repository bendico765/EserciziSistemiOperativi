/*
Scrivere un programma C che implementa una funzione
somma con la seguente segnatura:

int somma(int x);

La funzione 'somma' aggiunge l'intero x
letto dallo standard input (con scanf o fscanf) al valore calcolato
fino a quel momento, quindi ritorna la somma parziale.
Il numero di valori da leggere dallo standard input viene passato
come argomento del main.
Il valore iniziale della somma deve essere
'INIT_VALUE' che viene definito a compilazione
utilizzando il flag del preprocessore -D (es. -DINIT_VALUE=100).
Il programma non deve fare uso di variabili globali.

Valutare se la funzione somma così come definita è rientrante
ed eventualmente fornire una versione rientrante 'somma_r'.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifndef INIT_VALUE
#define INIT_VALUE 0
#endif

int somma(int x){
  static int partial_value = INIT_VALUE;
  partial_value += x;
  return partial_value;
}

int somma_r(int x, int *partial_value){
  *partial_value = *partial_value + x;
  return *partial_value;
}

/*
  La funzione riceve una stringa s in ingresso e prova a convertirla
  in long (conservando il valore in n), restituendo un
  codice per segnalare l'esito dell'operazione

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

int main(int argc, char *argv[]){
  long n;
  if(argc != 2 || (isNumber(argv[1], &n) != 0)){
    exit(EXIT_FAILURE);
  }
  int reentrant_result = INIT_VALUE;
  int not_reentrant_result;
  for(int i = 0; i < n; i++){
    int x;
    scanf("%d", &x);
    not_reentrant_result = somma(x);
    somma_r(x, &reentrant_result);
  }
  printf("Risultato non rientrante: %d\n", not_reentrant_result);
  printf("Risultato rientrante: %d\n", reentrant_result);
  return 0;
}
