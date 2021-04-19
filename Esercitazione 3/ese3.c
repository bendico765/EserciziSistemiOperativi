/*
scrivere le 3 macros CHECK_PTR_EXIT, ELEM e PRINTMAT.

'CHECK_PTR_EXIT' prende due argomenti, un puntatore ed una stringa.
La macro controlla che il primo argomento sia diverso da 'NULL'.
Solo se il puntatore è 'NULL' stampa utilizzando la funzione di
librerie perror (man 3 'perror') la stringa passata come secondo
argomento prima di uscire dal programma con EXIT_FAILURE (man 3 'exit').

'PRINTMAT' stampa la matrice passata come primo argomento.
Al suo interno PRINTMAT usa la macro ELEM per accedere agli elementi
della matrice (es. ELEM(M,3,4) equivale ad accedere
all'elemento M[3][4] della matrice M
– attenzione però che la matrice è allocata in modo contiguo
in memoria e per righe).
*/
#include <stdlib.h>
#include <stdio.h>
#define dimN 16
#define dimM  8
#define CHECK_PTR_EXIT(ptr, msg)\
if (ptr == NULL ){perror(msg);exit(EXIT_FAILURE);}
#define ELEM(M, i, j) M[(i*dimM) + j]
#define PRINTMAT(M, dim1, dim2)\
for(size_t i=0;i<dim1;++i){\
  for(size_t j=0;j<dim2;++j){printf("%ld\t",ELEM(M,i,j));}\
  printf("\n");}

int main() {
    long *M = malloc(dimN*dimM*sizeof(long));
    CHECK_PTR_EXIT(M, "malloc");
    for(size_t i=0;i<dimN;++i){
    	for(size_t j=0;j<dimM;++j){
    	    ELEM(M,i,j) = i+j;
      }
    }
    PRINTMAT(M, dimN, dimM);
    free(M);
    return 0;
}
