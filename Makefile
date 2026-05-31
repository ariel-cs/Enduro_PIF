CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -MMD -MP

# raylib precisa de bibliotecas de sistema diferentes em cada SO.
# No Windows assume-se o ambiente MSYS2/MinGW (shell Unix-like).
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET = enduro.exe
else
    LDFLAGS = -lraylib -lm -lpthread -ldl -lrt
    TARGET = enduro
endif

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
TEST_ENEMIES = tests/test_enemies

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

$(TEST_ENEMIES): tests/test_enemies.c src/enemy.c src/collision.c src/player.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: $(TEST_ENEMIES)
	./$(TEST_ENEMIES)

valgrind-enemies: $(TEST_ENEMIES)
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_ENEMIES)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET) $(TEST_ENEMIES)

.PHONY: run test valgrind-enemies clean

-include $(DEP)
