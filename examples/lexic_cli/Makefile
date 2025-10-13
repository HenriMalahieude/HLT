CC = gcc

_BUILD = ../../_build

BFLAGS = -I ../../src/Lexic/ -L $(_BUILD)/lib/ -l lexic
WFLAGS = -Wall
FLAGS = $(WFLAGS) $(BFLAGS)

default: $(_BUILD)/lib/liblexic.a
	$(CC) -o lexic_cli ./main.c $(FLAGS)

$(_BUILD)/lib/liblexic.a:
	@$(MAKE) -s -C ../../ Lexic

clean: 
	-@rm lexic_cli.exe
