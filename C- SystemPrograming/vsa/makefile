BIN=vsa
CODEDIRS=. src/
INCDIRS=. ../../include/

CC=gcc
OPT=-o3
DEBUG = -DNDEBUG $(OPT)
# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD
# automaticaly add -I (wich lib to search) onto each directory 
CFLAGS=-ansi -pedantic-errors -Wall -Wextra $(foreach D,$(INCDIRS),-I$(D)) $(DEPFLAGS)

# wildcard - find all the .c
CFILES=$(foreach D, $(CODEDIRS),$(wildcard $(D)/*.c))

#convert .c to .o
OBJS=$(patsubst %.c,%.o,$(CFILES))
#heandle the .h changes to .d
DEPEFILES=$(patsubst %.c,%.d,$(CFILES))

all: $(BIN) 

.PHONY: clean

$(BIN):$(OBJS)
	$(CC) -o $@ $^

%.o:%.c 
	$(CC) $(CFLAGS) -g -c -o $@ $<
	
debug:%.c
	$(CC) $(CFLAGS) $(DEBUG) -c -o DB$(BIN) $^
	
release:%.c
	$(CC) $(CFLAGS) $(DEBUG) -c -o Rel$(BIN) $^

clean:
	rm -rf $(OBJS) $(DEPEFILES)
	
-include $(DEPEFILES)

