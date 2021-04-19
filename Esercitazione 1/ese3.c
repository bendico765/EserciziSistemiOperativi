#include <stdio.h>
#include <stdlib.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val;
   return -1;
}

int main(int argc, char *argv[]){
  if( strcmp(argv[argc-1],"-h") == 0 ){
    printf("nome-programma -n <numero> -s <stringa> -m <numero> -h\n");
  }
  else{
    for(int i = 0; i < argc; i++){

    }
  }
  return 0;
}
