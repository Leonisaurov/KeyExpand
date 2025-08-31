BIN		:= key_expand
CONF	:= keyexpand.conf
DEV ?= FALSE
ifeq (${DEV}, TRUE)
	CFLAGS =-fsanitize=address -Wall -DDEBUG -g
else
	CFLAGS =
endif

SRC=$(wildcard *.c)
OBJ = $(SRC:.c=.o)

XDG_CONFIG_HOME ?= $(HOME)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

run: $(BIN)
	./$(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BIN)
	cp $(BIN) $(PREFIX)/bin
	mkdir -p $(XDG_CONFIG_HOME)/.config/
	[ -f $(XDG_CONFIG_HOME)/.config/$(CONF) ] || \
	cp $(CONF) $(XDG_CONFIG_HOME)/.config/

uninstall:
	rm $(PREFIX)/bin/$(BIN)
	rm $(XDG_CONFIG_HOME)/.config/$(CONF)

clean:
	rm -rfv $(OBJ) $(BIN) log.txt
