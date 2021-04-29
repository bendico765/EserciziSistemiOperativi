PARAMS = -Wall -std=c99
LIBS = -lpthread

TARGETS = server \
		client

all: $(TARGETS)

%: %.c
	gcc $(PARAMS) $^ -o $@ $(LIBS)
