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

int inputControl(int argc, char *argv[], long *numFilosofi){
	if( argc != 2 ){
		fprintf(stderr, "%s [numero filosofi]\n", argv[0]);
		return -1;
	}
	if( isNumber(argv[1], numFilosofi) != 0){
		fprintf(stderr, "Numero non valido\n");
		return -1;
	}
	if( *numFilosofi < 5 ){
		fprintf(stderr, "Il numero di filosofi deve essere >= 5\n");
		return -1;
	}
	return 0;
}

