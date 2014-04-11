NAME=claudia
CC=gcc
CFLAGS= -std=c99 -pthread -Wall -Wsign-compare -Winline -Wno-unused -Ofast -flto
DEPS=make.dep
CSRCS=$(wildcard *.c)
HSRCS=$(wildcard *.h)
OBJS=$(CSRCS:.c=.o)

.PHONY: all clean

all: $(NAME) $(DEPS)

$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o make.dep

include $(DEPS)

make.dep: $(CSRCS) $(HSRCS)
	$(CC) -MM $(CSRCS) > make.dep

