CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRC = login.c uart.c http.c
OBJ = $(SRC:.c=.o)
EXEC = output

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

run: $(EXEC)
	./$(EXEC)
