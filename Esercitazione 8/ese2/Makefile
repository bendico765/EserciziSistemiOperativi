FLAGS = -Wall -std=c99

exe: dec main.c
	gcc $(FLAGS) main.c -o $@
	
dec: dec.c
	gcc $(FLAGS) $^ -o $@
