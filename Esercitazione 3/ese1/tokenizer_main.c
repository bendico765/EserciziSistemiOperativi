/*
Inserire in un file 'tokenizer.h' la dichiarazione di due funzioni 'tokenizer' e 'tokenizer_r' 
(vedere la soluzione proposta nell'Esercitazione 2). 
'tokenizer' ha la stessa interfaccia di 'tokenizer_r' ma mentre la prima usa al suo interno 'strtok', 
la seconda usa 'strtok_r'. Inserire nel file 'tokenizer_lib.c' l'implementazione delle due funzioni (cioè la loro definizione). 
Creare quindi una librerie statica 'libtokenizer.a' che offre l'implementazione delle due funzioni. 
Scrivere un programma main (es. tokenizer_main.c) che utilizza una delle due (o entrambe le) funzioni, 
il programma includerà il file 'tokenizer.h' e verrà linkato alla libreria statica 'libtokenizer.a'.
*/

#define _POSIX_C_SOURCE 200112L
#include "tokenizer.h"
// esempio di tokenizzazione di stringhe

int main(int argc, char *argv[]) {
    for(int i=1;i<argc;++i)
	   tokenizer_r(argv[i]);
    return 0;
}
