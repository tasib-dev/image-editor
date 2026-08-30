CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -I/opt/iup/include

LDFLAGS = -L/opt/iup

LIBS = -liup 

TARGET = image-editor

SRC = $(wildcard src/*.c) \
      $(wildcard src/operations/*.c)

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS)