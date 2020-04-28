CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic 
LIBS = 

MAIN_OBJS_DIR = ../build/.objs

_MAIN_OBJS := $(shell ls ../build/.objs/ | grep .o | grep -v main.o)
MAIN_OBJS = $(patsubst %,$(MAIN_OBJS_DIR)/%,$(_MAIN_OBJS))


TEST_SRC_DIR = .
TEST_INCLUDE_DIR = $(TEST_SRC_DIR)/test_include
TEST_BUILD_DIR = ./test_build
TEST_OBJS_DIR = $(TEST_BUILD_DIR)/.objs
TEST_EXEC = all_test

_TEST_DEPS := $(shell ls $(TEST_INCLUDE_DIR))
TEST_DEPS = $(patsubst %,$(TEST_INCLUDE_DIR)/%,$(_DEPS))

_TEST_OBJS = unity.o test.o test_val.o test_parser.o test_list.o test_environment.o test_evaluator.o
$(shell mkdir -p $(TEST_OBJS_DIR))
TEST_OBJS = $(patsubst %,$(TEST_OBJS_DIR)/%,$(_TEST_OBJS))

$(TEST_OBJS_DIR)/%.o: $(TEST_SRC_DIR)/%.c $(TEST_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_EXEC): $(TEST_OBJS)
	cd .. && make -s
	$(CC) -o $(TEST_BUILD_DIR)/$@ $(MAIN_OBJS) $^ $(CFLAGS) $(LIBS)
	$(TEST_BUILD_DIR)/$(TEST_EXEC)

.PHONY: clean

clean:
	rm -rf $(TEST_OBJS_DIR) $(TEST_BUILD_DIR)/$(TEST_EXEC)
