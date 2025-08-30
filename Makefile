BIN ?= terminal_interceptor
DEV ?= FALSE
ifeq (${DEV}, TRUE)
	CFLAGS =-fsanitize=address -Wall
else
	CFLAGS =
endif

SRC=$(wildcard *.c)
OBJ = $(SRC:.c=.o)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rfv $(OBJ) $(BIN)
