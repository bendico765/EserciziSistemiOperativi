CFLAGS = -Wall -std=c99
INCLUDES = -I ./includes
LIBS = -lpthread

exe: main.c queueUtils.c threadUtils.c
	gcc $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
