CFLAGS = -Wall -std=c99
INCLUDES = -I ./includes
LIBS = -lpthread -lm

exe: main.o threadUtils.o queueUtils.o utils.o
	gcc $(CFLAGS) $^ -o $@ $(LIBS)
	
main.o: main.c
	gcc $(CFLAGS) $(INCLUDES) $^ -c -o $@ $(LIBS)
	
threadUtils.o: threadUtils.c
	gcc $(CFLAGS) $^ -c -o $@
	
queueUtils.o: queueUtils.c
	gcc $(CFLAGS) $^ -c -o $@

utils.o: utils.c
	gcc $(CFLAGS) $^ -c -o $@
