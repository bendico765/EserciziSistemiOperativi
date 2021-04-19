CFLAGS = -Wall -std=c99
INCLUDES = -I ./includes
LIBS = -lpthread

exe: main.c threadUtils.c queueUtils.c
	gcc $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
