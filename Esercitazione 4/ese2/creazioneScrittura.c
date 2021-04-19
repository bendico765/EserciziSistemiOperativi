/*
Scrivere un programma che prende in ingresso un intero 'N' ('N' e' piccolo,
es. <=512), alloca in memoria una matrice di NxN elementi di tipo float
 in modo che siano contigui in memoria. Inizializzare tale matrice
 (chiamata ad esempio M1) nel modo seguente M1(i,j) = (i+j)/2.0.

Fare quindi il salvataggio in un file della matrice M1 sia in formato binario
che in formato testuale in modo da ottenere due file separati rispettivamente
con nome 'mat_dump.dat' per il formato binario, e 'mat_dump.txt' per quello
testuale. Chiudere i file creati.

Scrivere un secondo programma (oppure estendere quello precedente)
che prende in ingresso i nomi dei due file creati in precedenza e l'intero 'N',
legge la matrice sia dal file 'mat_dump.dat' che dal file 'mat_dump.txt'
memorizzandone il contenuto in due matrici distinte (ad esempio M1 ed M2).
Al termine dell'operazione di lettura, effettuare il confronto delle due
matrici con la funzione 'confronta' che deve essere implementata in modo
“generico” cioe' deve prendere come argomenti un puntatore a funzione con
tipo opportuno, i puntatori alle due matrici da confrontare e la size delle
matrici. Il programma deve stampare l'esito del confronto.
Testare il programma passando alla funzione 'confronta'
la funzione di libreria memcmp (man 3 memcmp).


Notare la differenza di size tra 'mat_dump.dat' e 'mat_dump.txt'.
Suggerimento: per leggere e scriver un file binario utilizzare rispettivamente
le funzioni di libreria fread ed fwrite.
Per scrivere il file testuale usare la chiamata fprintf.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define MAX_N 512
#define BIN_FILENAME "mat_dump.dat"
#define TXT_FILENAME "mat_dump.txt"
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

int writeTextFile(char *filename, size_t size ,float M1[][size]){
  FILE *file = fopen(filename, "w");
  if( file == NULL ){
    return -1;
  }
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      fprintf(file, "%f\n", M1[i][j]);
    }
  }
  fclose(file);
  return 0;
}

int writeBinaryFile(char *filename, size_t size ,float M1[][size]){
  FILE *file = fopen(filename, "wb");
  if( file == NULL ){
    return -1;
  }
  for(int i = 0; i < size; i++){
    for(int j = 0; j < size; j++){
      fwrite( (const void*)(&M1[i][j]) , sizeof(float), 1, file);
    }
  }
  fclose(file);
  return 0;
}

int main(int argc, char *argv[]){
  if( argc != 2 ){
    printf("Wrong number of arguments\n");
    exit(EXIT_FAILURE);
  }
  long n;
  if( isNumber(argv[1], &n) != 0 || n > MAX_N){
    printf("Error with the second argument\n");
    exit(EXIT_FAILURE);
  }
  float M1[n][n];
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      M1[i][j] = (i+j)/2.0;
    }
  }
  if( writeTextFile(TXT_FILENAME, n, M1) == 0 ){
    printf("Txt file successfully created\n");
  }
  else{
    printf("Something went wrong with the txt file!\n");
  }
  if( writeBinaryFile(BIN_FILENAME, n, M1) == 0 ){
    printf("Binary file successfully created\n");
  }
  else{
    printf("Something went wrong with the binary file!\n");
  }
  return 0;
}
