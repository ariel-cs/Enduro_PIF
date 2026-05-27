CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = enduro
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
	rm -f $(OBJ) $(TARGET) $(TEST_ENEMIES)

.PHONY: run test valgrind-enemies clean
