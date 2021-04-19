#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	if( argc != 2 ){
		printf("./zombie N\n");
		return 0;
	}
	int N = atoi(argv[1]);
	for(int i = 0; i < N; i++){
		pid_t pid = fork();
		if( pid == -1){
			perror("Si è verificato un errore con la fork\n");
			exit(-1);
		}
		if( pid == 0 ){ // processo figlio
			return 0;
		}
	}
	sleep(20);
	return 0;
}
