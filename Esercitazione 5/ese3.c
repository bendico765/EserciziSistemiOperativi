/*
Scrivere un programma C che dati come argomenti una lista di file o
directories stampa per ogni argomento: il nome del file/directory,
il numero dell'inodo associato, il tipo di file (-,l,d,s,…) i bit di
protezione (es. rw-r–r–), l'user identifier (uid), il group identifier (gid),
la size del file ed il timestamp dell'ultima modifica.
Per convertire il tempo di ultima modifica in un formato stampabile
usare la funzione di libreria ctime. Usare getpwuid e getgrgid per
convertire uid e gid nei nomi corrispondenti agli id.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#define STR_MAX_LEN 256
#define perror_null(var, msg)\
  if( var == NULL ){ perror(msg); exit(-1);}

char* strndup(const char *s, size_t n);

char getMode(mode_t st_mode){
  if (S_ISREG(st_mode)) return 'f';
  if (S_ISDIR(st_mode)) return 'd';
  if (S_ISCHR(st_mode)) return 'c';
  if (S_ISBLK(st_mode)) return 'b';
  if (S_ISLNK(st_mode)) return 'l';
  if (S_ISFIFO(st_mode)) return 'p';
  if (S_ISSOCK(st_mode)) return 's';
  return '-';
}

void getPermission(mode_t st_mode, char mode[]){
  if (S_IRUSR & st_mode) mode[0]='r';
  if (S_IWUSR & st_mode) mode[1]='w';
  if (S_IXUSR & st_mode) mode[2]='x';

  if (S_IRGRP & st_mode) mode[3]='r';
  if (S_IWGRP & st_mode) mode[4]='w';
  if (S_IXGRP & st_mode) mode[5]='x';

  if (S_IROTH & st_mode) mode[6]='r';
  if (S_IWOTH & st_mode) mode[7]='w';
  if (S_IXOTH & st_mode) mode[8]='x';
}


int printStats(char *filename){
  struct stat statbuf;
  errno = 0;
  if( stat(filename, &statbuf) == -1 ){
    return -1;
  }
  char permissions[10] = {'-','-','-','-','-','-','-','-','-','\0'};
  getPermission(statbuf.st_mode, permissions);
  printf("Name: %s\n", filename);
  printf("I-node number: %ld\n", statbuf.st_ino);
  printf("File type and mode: %c\n", getMode(statbuf.st_mode));
  printf("Permission: %s\n", permissions);
  printf("User: %s\n", (getpwuid(statbuf.st_uid))->pw_name );
  printf("Group: %s\n", (getgrgid(statbuf.st_gid))->gr_name );
  printf("Size (bytes): %ld\n", statbuf.st_size);
  printf("Last modified: %s", ctime(&statbuf.st_mtime));
  printf("-----\n");
  return 0;
}

int main(int argc, char *argv[]){
  int arraysize = argc-1;
  char *array[arraysize];
  for(int i = 0; i < arraysize; i++){
    errno = 0;
    array[i] = strndup(argv[i+1], STR_MAX_LEN);
    perror_null(array[i], "Error with input data\n");
  }
  for(int i = 0; i < arraysize; i++){
    if( printStats(array[i]) == -1 ){
      printf("Error while getting stats from: %s\n", array[i]);
      perror(NULL);
      break;
    }
  }
  for(int i = 0; i < arraysize; i++){
    free(array[i]);
  }
  return 0;
}
