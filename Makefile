SRC_DIR=src
BIN_DIR=bin
DOC_DIR=doc

.PHONY: test
	test

.PHONY: doc
	doc

all: setup doc
	gcc -g $(SRC_DIR)/*.c -o $(BIN_DIR)/main

doc:
	doxygen Doxyfile 

setup:
	@mkdir -p $(BIN_DIR)

test: setup
	@mkdir -p test/support
	@ceedling clobber
	@ceedling gcov:all
	@ceedling utils:gcov

clean:
	rm -rf $(BIN_DIR)/
	rm -rf $(DOC_DIR)/
