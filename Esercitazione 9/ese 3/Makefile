PARAMS = -Wall -std=c99
INCLUDES = -I ./includes
TARGETS = server \
		client

all: $(TARGETS)

%: utils.c %.c
	gcc $(PARAMS) $(INCLUDES) $^ -o $@

