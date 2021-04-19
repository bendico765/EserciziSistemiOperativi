/*
Scrivere un programma C chiamato 'lsdir' che, dato come
argomento un nome di directory, visita ricorsivamente
tutto il sottoalbero di directory che ha come radice la
directory passata come argomento. Per ogni directory,
il programma deve stampare sullo standard output le
informazioni sui file nel seguente formato:

Directory: <nomedir1>
file1     size    permessi
file2     size    permessi
------------------
Directory: <nomedir2>
file3     size    permessi
file4     size    permessi
------------------
dove la directory 'nomedir1' contiene i file 'file1' e 'file2'
e la directory 'nomedir2'. Durante la visita dell'albero di
directory non cosiderare le directories
'.' (punto) e '..' (punto-punto).
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define STR_MAX_LEN 256
#define perror_null(var, msg)\
  if( var == NULL ){perror(msg); exit(-1);}
#define perror_less1(var, msg)\
  if( var == -1 ){perror(msg); exit(-1);}

char *strndup(const char *s, size_t n);

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

void printFileInfo(char *filename, struct stat *statbuf){
  char permissions[10] = {'-','-','-','-','-','-','-','-','-','\0'};
  getPermission(statbuf->st_mode, permissions);
  printf("%60s\t%10ld\t%s\n", filename, statbuf->st_size, permissions);
}

void lsdir(char *dirname){
  DIR *dir = opendir(dirname);
  perror_less1(chdir(dirname), "Error while entering directory\n");
  perror_null(dir, "Error while opening dir\n");

  struct dirent *file;
  struct stat statbuf;
  printf("Directory: %s\n", dirname);
  while( (errno = 0,file = readdir(dir)) != NULL ){
    if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
      perror_less1(stat(file->d_name, &statbuf), "Error while retriving file data\n");
      if(S_ISDIR(statbuf.st_mode)){
        lsdir(file->d_name);
      }
      else{
        printFileInfo(file->d_name, &statbuf);
      }
    }
  }
  if( errno != 0){
    perror("Something wrong while reading files\n");
    exit(-1);
  }
  perror_less1(closedir(dir),"Something wrong while closening directory\n");
  perror_less1(chdir(".."), "Something wrong while exiting from directory\n");
  printf("----------\n");
}

int main(int argc, char *argv[]){
  if( argc != 2 ){
    printf("lsdir <nome-directory>\n");
    exit(-1);
  }
  errno = 0;
  char *dirname = strndup(argv[1], STR_MAX_LEN);
  perror_null(dirname, "Error with dir name\n");

  lsdir(dirname);

  free(dirname);
  return 0;
}
