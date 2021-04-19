#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* strtok_r(char *str, const char *delim, char **saveptr);

void tokenizer_r(char *stringa) {
    char *tmpstr;
    char *token = strtok_r(stringa, " ", &tmpstr);
    while (token) {
      printf("%s\n", token);
      token = strtok_r(NULL, " ", &tmpstr);
    }
}
void tokenizer(char *stringa) {
  char* token = strtok(stringa, " ");
  while (token) {
    printf("%s\n", token);
    token = strtok(NULL, " ");
  }
}
