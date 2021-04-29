CFLAGS = -Wall -std=c99

TARGETS = server \
		client

all: $(TARGETS)

%: %.c
	gcc $(CFLAGS) $^ -o $@


