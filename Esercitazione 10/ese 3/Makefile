PARAMS = -Wall
INCLUDES = -I ./includes
LIBS = -pthread
TARGETS = server \
		client

all: $(TARGETS)

%: utils.c %.c
	gcc $(PARAMS) $(INCLUDES) $^ -o $@ $(LIBS)

