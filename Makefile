CC = gcc
CFLAGS = -Wall
LDFLAGS = -lrt


EXEC = ctimer
OBJS = ctimer.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) 


clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
 