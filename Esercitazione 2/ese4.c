/*
  Generare N numeri casuali interi nell'intervallo [K1,K2[ utilizzando
  la funzione rand_r(). N, K1 e K2 sono passati come argomenti del main opzionali,
  e se non forniti dall'utente assumono valori di default sulla base di opportune
  #define (es. N=1000 K1=100 K2=120). Il programma deve restituire il numero di
  occorrenze di ciascun intero i nell'intervallo [K1,K2[e stamparle sullo standard output.
  Un esempio di output prodotto considerando K1=0 e K2=10, è il seguente:
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef N
  #define N 10
#endif
#ifndef K1
  #define K1 20
#endif
#ifndef K2
  #define K2 30
#endif

int rand_r(unsigned int *seedp);

int main(int argc, char *argv[]){
  unsigned int seed = time(NULL);
  int numbers = K2 - K1;
  int results[numbers];
  for(int i = 0; i < numbers; i++){ results[i] = 0; }
  for(int i = 0; i < N; i++){
    int random_number = K1 + rand_r(&seed) % (K2 - K1); // numeri tra [K1, K2)
    results[random_number - K1] += 1;
  }
  for(int i = 0; i < numbers; i++){
    printf("Numbero: %2d\tOccorrenze: %2d\n", i+K1, results[i]);
  }
  return 0;
}
