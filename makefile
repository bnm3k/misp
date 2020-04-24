CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror  -pedantic 
LIBS = 

SRC_DIR = src

BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/.objs
MAIN_EXEC = misp

_DEPS = val.h
DEPS = $(patsubst %,$(SRC_DIR)/%,$(_DEPS))

_OBJS = main.o val.o
$(shell mkdir -p $(OBJS_DIR))
OBJS = $(patsubst %,$(OBJS_DIR)/%,$(_OBJS))

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(MAIN_EXEC): $(OBJS)
	$(CC) -o $(BUILD_DIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: test

test:
	@cd test && make -s

.PHONY: clean

clean:
	rm -rf $(OBJS_DIR) $(BUILD_DIR)/$(MAIN_EXEC)
