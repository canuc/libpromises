CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -fPIC -c 
SOURCES=promises.c
OBJ_DIR=objects
OBJECTS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
LIB=libpromises.a
	
all: $(SOURCES) $(LIB)

$(LIB): $(OBJECTS)
	ar rcs $@ $(OBJECTS) 

$(OBJ_DIR)/%.o: %.c $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

.PHONY: $(OBJ_DIR) all