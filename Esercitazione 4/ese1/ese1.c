/*
Scrivere un programma C che legge il file testuale “/etc/passwd” e scrive in un
file testuale la lista dei 'login name' del sistema, uno per riga.
Il nome del file testuale di output viene passato come unico argomento al programma.
Il formato del file “/etc/passwd” è descritto nella sezione 5 del manuale
entry 'passwd' (man 5 passwd) : “/etc/passwd contains one line for each user account,
with seven fields delimited by colons (”:“).”
Il primo campo è il 'login name'.
Suggerimento: usare fgets per leggere le righe del file; usare strchr per
trovare la prima occorrenza di ':' nella stringa letta dal file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024

char *strtok_r(char *str, const char *delim, char **saveptr);

int main(int argc, char *argv[]){
  if( argc != 2 ){
    printf("Output filename required\n");
    exit(EXIT_FAILURE);
  }
  char *outputFileName = argv[1];
  FILE *fr = fopen("/etc/passwd", "r");
  FILE *fw = fopen(outputFileName, "w");
  if( fr == NULL){
    printf("Failed to open /etc/passwd\n");
    exit(EXIT_FAILURE);
  }
  if( fw == NULL){
    printf("Failed to open output file\n");
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE + 1];
  while( fgets(buffer, BUFFER_SIZE, fr) != NULL ){
    char *temp = NULL;
    strtok_r(buffer, ":", &temp);
    fprintf(fw, "%s\n", buffer);
  }
  fclose(fr);
  fclose(fw);
  return 0;
}
