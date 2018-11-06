
#Makefile for program hello
 
program := test_ringBuffer
 
sources := $(wildcard *.c)
headers := $(wildcard *.h)
 
objects := $(sources:.c=.o)
deps    := $(sources:.c=.d)
 
CC      := g++
SHARED  := -shared
FPIC    := -fPIC
CFLAGS  := -g -Wall -Werror -pthread -I./Include
LDFLAGS := 
 
RM      := rm -f

me      := Makefile
 
all:    $(program)
 
test_ringBuffer:  $(objects)
		$(CC) -o $@ $(CFLAGS) $(objects) $(LDFLAGS) $(LDLIBS)

clean:
		$(RM) $(objects)
		$(RM) $(deps)
		$(RM) $(program)

-include $(deps)
