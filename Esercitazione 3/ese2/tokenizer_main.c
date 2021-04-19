/*
Come nell'Esercizio 1 però invece che una libreria statica creare
una libreria condivisa ('libtokenizer.so').
Verificarne il corretto caricamento in memoria della libreria
spostando il file 'libtokenizer.so' in una directory differente
da quella corrente (es. ~/mylibdir). Provare ad usare sia l'opzione
del linker -Wl, -rpath, che la variabile d'ambiente per il
dynamic linker/loader LD_LIBRARY_PATH.
*/

#define _POSIX_C_SOURCE 200112L
#include "tokenizer.h"

int main(int argc, char *argv[]) {
    for(int i=1;i<argc;++i)
	   tokenizer_r(argv[i]);
    return 0;
}
