/*
Realizzare il comando myfind ispirato al comando di shell find

myfind dir nomefile

che cerca ricorsivamente il file nomefile nel sottoalbero radicato
nella directory dir. Per ogni file nomefile trovato, stampa il path
assoluto della directory in cui e' stato trovato e la data dell'ultima
modifica.
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#define STR_MAX_LEN 256
#define perror_null(var, msg)\
  if( var == NULL ){perror(msg); exit(-1);}

char *strndup(const char *s, size_t n);

void find(char *dirName, char *filename){
  errno = 0;
  DIR *dir = opendir(dirName);
	perror_null(dir, "Error while opening directory\n");
  chdir(dirName);
  perror_null(dir, "Error while entering directory\n");

  struct dirent *file;
  char buffer[STR_MAX_LEN];
  while( (errno = 0, file = readdir(dir)) != NULL ){
    if( strncmp(file->d_name, filename, STR_MAX_LEN) == 0 ){
      perror_null(getcwd(buffer, STR_MAX_LEN), "Error with cwd name\n");
      printf("Path: %s\n", buffer);
    }
    if( file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
      find(file->d_name, filename);
    }
  }
  if( errno != 0 ){
    perror("Error while reading directory content\n");
    exit(-1);
  }
  errno = 0;
  if( closedir(dir) == -1 ){
    perror("Error while closening directory\n");
    exit(-1);
  }
  chdir("..");
}

int main(int argc, char *argv[]){
  if( argc != 3 ){
    printf("myfind dir nomefile\n");
    exit(-1);
  }
	char *dirName = argv[1];
	char *filename = argv[2];

  find(dirName, filename);

  return 0;
}
