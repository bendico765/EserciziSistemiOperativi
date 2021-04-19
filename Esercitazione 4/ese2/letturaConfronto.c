#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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

int confronta(int (*fun) (const void*, const void*, size_t), float *M1, float *M2, size_t size){
  return fun((const void*)M1, (const void*)M2, size*size);
}

// ./letturaConfronto <nome-file-txt> <nome-file-bin> <dimensione matrice>
int main(int argc, char *argv[]){
  if( argc != 4 ){
    printf("Wrong number of arguments\n");
    exit(EXIT_FAILURE);
  }
  FILE *txtFile = fopen(argv[1], "r");
  FILE *binFile = fopen(argv[2], "rb");
  long n;
  if( isNumber(argv[3], &n) != 0 ){
    printf("Error with the third argument\n");
    exit(EXIT_FAILURE);
  }
  if( txtFile == NULL ){
    printf("Impossibile to open txt file!\n");
    exit(EXIT_FAILURE);
  }
  if( binFile == NULL ){
    printf("Impossibile to open bin file!\n");
    exit(EXIT_FAILURE);
  }
  float *M1 = malloc(sizeof(float) * (n*n));
  float *M2 = malloc(sizeof(float) * (n*n));
  if( M1 == NULL || M2 == NULL){
    printf("Malloc failed!\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      float txtValue, binValue;
      fscanf(txtFile, "%f", &txtValue);
      fread((void*)(&binValue), sizeof(float), 1, binFile);
      M1[i*n + j] = txtValue;
      M2[i*n + j] = binValue;
    }
  }

  if( confronta(&memcmp, M1, M2, n) == 0 ){
    printf("Le matrici sono uguali\n");
  }
  else{
    printf("Le matrici sono diverse\n");
  }

  free(M1);
  free(M2);
  fclose(txtFile);
  fclose(binFile);
  return 0;
}
