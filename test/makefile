CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic 
LIBS = 

# SRC_DIR = ../src

# BUILD_DIR = ../build
# OBJS_DIR = $(BUILD_DIR)/.objs
# MAIN_EXEC = misp

TEST_SRC_DIR = .
TEST_BUILD_DIR = ./test_build
TEST_OBJS_DIR = $(TEST_BUILD_DIR)/.objs
TEST_EXEC = all_test



_TEST_DEPS = unity.h unity_internals.h
TEST_DEPS = $(patsubst %,$(TEST_SRC_DIR)/%,$(_DEPS))

_TEST_OBJS = test.o unity.o
$(shell mkdir -p $(TEST_OBJS_DIR))
TEST_OBJS = $(patsubst %,$(TEST_OBJS_DIR)/%,$(_TEST_OBJS))

$(TEST_OBJS_DIR)/%.o: $(TEST_SRC_DIR)/%.c $(TEST_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_EXEC): $(TEST_OBJS)
	$(CC) -o $(TEST_BUILD_DIR)/$@ $^ $(CFLAGS) $(LIBS)
	$(TEST_BUILD_DIR)/$(TEST_EXEC)

.PHONY: clean

clean:
	rm -rf $(TEST_OBJS_DIR) $(TEST_BUILD_DIR)/$(TEST_EXEC)
