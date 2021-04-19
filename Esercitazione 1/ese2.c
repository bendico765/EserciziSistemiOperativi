#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int REALLOC_INC=16;

void RIALLOCA(char** buf, size_t newsize) {
  *buf = realloc(*buf, newsize);
}
char* mystrcat(char *buf, size_t sz, char *first, ...) {
  va_list argList;
  va_start(argList,first);
  size_t bufferSize = sz;

  if( strlen(first) > bufferSize ){
    bufferSize = strlen(first)+1; // +1 per il carattere \0
    RIALLOCA(&buf, bufferSize);
  }
  strcpy(buf, first);

  char *ptrArg;
  while( (ptrArg = va_arg(argList, char*)) != NULL ){
    bufferSize += strlen(ptrArg);
    RIALLOCA(&buf, bufferSize);
    strcat(buf, ptrArg);
  }
  va_end(argList);
  return buf;
}

int main(int argc, char *argv[]) {
  if (argc < 7) { printf("troppi pochi argomenti\n"); return -1; }
  char *buffer=NULL;
  RIALLOCA(&buffer, REALLOC_INC);  // macro che effettua l'allocazione del 'buffer'
  buffer[0]='\0'; // mi assicuro che il buffer contenga una stringa
  buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
  printf("%s\n", buffer);
  free(buffer);
  return 0;
}
