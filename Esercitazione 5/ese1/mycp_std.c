/*
Scrivere un programma, utilizzando chiamate di sistema, che implementi
l'equivalente del comando Unix cp.
Il programma deve accettare 3 argomenti come segue:

mycp_sc filein fileout [buffersize]

L'argomento 'buffersize' e' la dimensione del buffer da utilizzare per le
letture e scritture con le SCs read e write (se non specificato assegnare un
valore di default, es. 256bytes): Realizzare quindi lo stesso programma,
utilizzando le chiamate di libreria fread ed fwrite. Chiamiamo questa
seconda versione 'mycp_std'. Confrontare le prestazioni (usando il comando
time da linea di comando) del programma 'mycp_sc' e 'mycp_std' al variare del
parametro 'buffersize'
(provare un pò di casi per buffersize: 1 4 8 16 32 ….. 8192). Che cosa si nota ?
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#define MAX_LENGHT 256
#define ec_null(var, msg)\
  if( var == NULL ){perror(msg); exit(-1);}
#define ec_not0(var, msg)\
  if( var != 0 ){perror(msg); exit(-1);}
#define ec_less1(var, msg)\
  if( var == -1 ){perror(msg); exit(-1);}

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

void mycopy(char *filein, char *fileout, long buffersize){
  char buffer[buffersize];
  FILE *in = fopen(filein, "r");
  ec_null(filein, "Impossibile to open input file\n");
  FILE *out = fopen(fileout, "w");
  ec_null(fileout, "Impossibile to open output file\n");

  int readChars;
  errno = 0;
  while( (readChars = fread(buffer, sizeof(char), buffersize, in)) > 0 ){
    fwrite(buffer, sizeof(char), readChars, out);
  }

  fclose(in);
  fclose(out);
}

int main(int argc, char *argv[]){
	if( argc != 4 ){
    printf("mycp_sc filein fileout [buffersize]\n");
    exit(-1);
  }
  long buffersize;
  char *filein = argv[1];
  char *fileout = argv[2];

	errno = 0;
  ec_not0(isNumber(argv[3], &buffersize), "Error with buffersize\n");
  mycopy(filein, fileout, buffersize);
  return 0;
}
