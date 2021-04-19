/*
Scrivere una funzione C che prende in input come primo argomento una stringa,
come secondo argomento la lunghezza della stringa e restituisca nel terzo argomento
la stessa stringa con tutti i sui caratteri in maiuscolo:

void strtoupper(const char* in, size_t len, char* out);

Scrivere il programma main di test per la funzione 'strtoupper' che prende
la/le stringa/e da passare alla funzione come argomenti da linea di comando.
Per convertire una lettera in maiuscolo si può usare 'toupper' (man 3 toupper).

*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void strtoupper(const char* in, size_t len, char* out){
  for(int i = 0; i < len; i++){
    out[i] = toupper(in[i]);
  }
}

int main(int argc, char *argv[]){
  for(int i = 1; i < argc; i++){
    char *in = argv[i];
    size_t len = strlen(in) + 1;
    char *out = malloc(sizeof(char) * len);
    if( out == NULL ){
      printf("Memoria insufficiente\n");
      exit(-1);
    }
    out[len-1] = '\0';
    strtoupper(in, len, out);
    printf("%s\n", out);
    free(out);
  }
  return 0;
}
