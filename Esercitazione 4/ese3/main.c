/*
Scrivere un programma che implementa una versione semplificata del comando Unix
wc (word count) limitatamente alle opzioni -l e -w (man 1 wc).
Usare getopt per il parsing degli argomenti.

Se non vengono passate opzioni, il programma stampa sia il numero di linee che
il numero di parole del/dei file i cui nomi sono passati come argomento

mywc [-l -w] file1 [file2 file3 ….]

Suggerimento: scrivere una funzione che conta le parole di una stringa
utilizzando le funzioni di utilità della libreria standard come isspace,
iscntrl (man 3 isspace).
*/
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "prototypes.h"

int main(int argc, char *argv[]){
  int opt;
  int lFlag = 0;
  int wFlag = 0;
  //opterr = 0; // disattiva la stampa degli errori
  while((opt = getopt(argc, argv, "lw")) != -1){
    switch(opt){
      case 'l':
        lFlag = 1;
        break;
      case 'w':
        wFlag = 1;
        break;
      case '?':
        printf("unknow option\n");
        break;
      default:
        break;
    }
  }
  int firstFilenameIndex = lFlag+wFlag+1;
  if(lFlag == 0 && wFlag == 0){
    lFlag = 1;
    wFlag = 1;
  }
  for(int i = firstFilenameIndex; i < argc; i++ )
  {
    char *filename = argv[i];
    printf("FILE: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if( file == NULL ){
      printf("Impossibile to open file.\n");
    }
    else{
      int linesNumber = 0;
      int wordsNumber = 0;
      char ch;
      while( fscanf(file,"%c",&ch) == 1 ){
        if( lFlag == 1 && iscntrl(ch) != 0 ){
          linesNumber++;
        }
        if( wFlag == 1 && isspace(ch) != 0){
          wordsNumber++;
        }
      }
      if( lFlag == 1){
        printf("Line count: %d\n", linesNumber);
      }
      if( wFlag == 1){
        printf("Word count: %d\n", wordsNumber);
      }
      printf("\n");
      if( file != NULL){
        fclose(file);
      }
    }
  }
  return 0;
}
