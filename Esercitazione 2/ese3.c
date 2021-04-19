#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 512

char *strndup(const char *, size_t);
char *strtok_r(char *str, const char *delim, char **saveptr);

int main(int argc, char *argv[]) {
    if (argc != 3) {
	    fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
	    return -1;
    }
	// faccio un controllo sulla lunghezza massima delle stringhe in input
    if ((strlen(argv[1]) > MAX_SIZE) || (strlen(argv[2]) > MAX_SIZE)) {
      printf("ERRORE: stringhe troppo lunghe\n");
      return -1;
    }
    char *saveptr1 = NULL;
    char* token1 = strtok_r(argv[1], " ", &saveptr1);
    while (token1) {
      printf("%s\n", token1);
	  char *copy2 = strndup(argv[2], MAX_SIZE);
      char *saveptr2 = NULL;
      char* token2 = strtok_r(copy2, " ", &saveptr2);
      while(token2) {
          printf("%s\n", token2);
          token2 = strtok_r(NULL, " ", &saveptr2);
      }
      token1 = strtok_r(NULL, " ", &saveptr1);
	  free(copy2);
    }
    return 0;
}
