/*
Scrivere un programma C che prende in ingresso 4 opzioni da linea di
comando codificate con '-n', '-m', '-o', '-h' nel modo seguente:
nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h

Il programma dovrà stampare sullo standard output le opzioni riconosciute
valide con il relativo argomento.
L'opzione '-h' non ha argomento e corrisponde al messaggio di uso.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int opt;
    //opterr = 0; // disattiva la stampa degli errori
    while((opt = getopt(argc, argv, ":n:m:o:h")) != -1){
      switch(opt){
        case 'n':
        case 'm':
        case 'o':
          printf("option: %c\t", opt);
          printf("argument: %s\n", optarg);
          break;
        case 'h':
          printf("nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h\n");
          break;
        case ':':
          printf("option with missing argument\n");
          break;
        case '?':
          printf("unknow option\n");
          break;
        default:
          break;
      }
    }
    return 0;
}
