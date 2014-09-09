
CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -g `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_mixer -lSDL_ttf -lm -lGL -lGLU

TARGET = main

SRC_PATH = src
OBJ_PATH = obj
INC_PATH = include
BIN_PATH = bin

SRC_FILES = $(shell find $(SRC_PATH) -type f -name '*.c')
OBJ_FILES = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SRC_FILES))

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_PATH)
	$(CC) -o $(BIN_PATH)/$(TARGET) $+ $(LDFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c $< -o $@ $(CFLAGS) -I $(INC_PATH)

clean:
	rm $(OBJ_FILES)

mrproper: clean
	rm $(BIN_PATH)/$(TARGET)
