CC := gcc
FLAGS := -Wall -Wextra -g -Og
XTRA_FLAGS ?=

HLP_DIR := ../../../helpers
SRC_DIR := ../../private

#In order of importance (foundational -> uses foundation)
DEPS := sets.o \
	warn.o \
	trim.o \
	file_to_str.o \
	terminal.o \
	book.o \
	firsts.o \
	follows.o \
	ll1.o \
	starting_rule.o

INDV_TESTS := set book_ruleadd book_fromx first follow ll1_table
INDV_EXE := $(addsuffix .out, $(INDV_TESTS))

.PHONY: all_indv clean help
.SUFFIXES: 
.NOTINTERMEDIATE: $(DEPS)

all_indv: $(INDV_EXE)

%.out: ../%.c $(DEPS)
	$(CC) -o $@ $^ $(FLAGS) -DVERBOSE $(XTRA_FLAGS) -I $(HLP_DIR)

#Helpers
sets.o: $(HLP_DIR)/sets/sets.c
	$(CC) -c $< $(FLAGS)

warn.o: $(HLP_DIR)/log/warn.c $(HLP_DIR)/log/warn.h
	$(CC) -c $< $(FLAGS)

file_to_str.o: $(HLP_DIR)/string/file_to_str.c
	$(CC) -c $< $(FLAGS)

trim.o: $(HLP_DIR)/string/trim.c
	$(CC) -c $< $(FLAGS)

%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(FLAGS) -I $(HLP_DIR)

#clean:
#-rm *.o 2>/dev/null || true
#-rm $(INDV_TESTS) 2>/dev/null || true
