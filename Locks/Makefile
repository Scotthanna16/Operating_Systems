CC     = gcc
INC    = -I.
CFLAGS = $(INC) -Wall -Wextra -Werror -O3
# CFILES = $(wildcard *.c)
# OBJS   = $(patsubst %.c, %.o,$(CFILES))
# BIN    = test

# %.o:%.c
# 	$(info Compiling $<)
# 	@$(CC) $(CFLAGS) -o $@ -c $<

# $(BIN):$(OBJS)
# 	$(CC) -o $@ $^

all: lfree mutex


lfree: main_free.c
	$(CC) $(CFLAGS) main_free.c -o lfree

mutex: main_mutex.c
	$(CC) $(CFLAGS) main_mutex.c -o mutex


clean:
	@rm -f lfree mutex
