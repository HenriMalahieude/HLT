CC = gcc
FLAGS = -Wall -Wextra -g -Og
XTRA_FLAGS ?=

HLP_DIR := ../../../helpers
SRC_DIR := ../../private

#Ordered in how foundational it is
DEPS := warn.o		\
	file_to_str.o	\
	trim.o		\
	match.o		\
	validate.o	\
	vocab.o		\
	name.o		\
	token.o

#Also ordered in foundational
INDV_TESTS := direct escaped brackets qualifier group or vocab name token
INDV_EXES := $(addsuffix .out, $(INDV_TESTS))

.PHONY: all all_indv clean
.SUFFIXES: 
.NOTINTERMEDIATE: $(DEPS)

all: $(DEPS)
	$(CC) -o $@.out ../all_test.c $(addsuffix .c,$(addprefix ../,$(INDV_TESTS))) $(DEPS) -I $(HLP_DIR) -DALL_TESTS

all_indv: $(INDV_EXES)

%.out: ../%.c $(DEPS)
	$(CC) -o $@ $^ $(FLAGS) -DVERBOSE $(XTRA_FLAGS) -I $(HLP_DIR)

#Helpers
warn.o: $(HLP_DIR)/log/warn.c $(HLP_DIR)/log/warn.h
	$(CC) -c $< $(FLAGS)

file_to_str.o: $(HLP_DIR)/string/file_to_str.c
	$(CC) -c $< $(FLAGS)

trim.o: $(HLP_DIR)/string/trim.c
	$(CC) -c $< $(FLAGS)

%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(FLAGS) -I $(HLP_DIR)

clean:
	@echo 'Removing all object and executables'
	@rm *.o 2>/dev/null || true
	@rm *.exe 2>/dev/null || true
	@rm *.out 2>/dev/null || true
