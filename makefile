NAME = rastarizer
CC = gcc
FLAGS = -std=c99 -pedantic -g
FLAGS+= -Wall -Wno-implicit-fallthrough -Wno-unused-parameter
FLAGS+= -Wextra -Werror=vla -Werror
VALGRIND = --show-leak-kinds=all --track-origins=yes --leak-check=full
CMD = ./$(NAME)

BIND = bin
OBJD = obj
SRCD = src
SUBD = sub
TESTD = tests

INCL = -I$(SRCD)
INCL+= -I$(SUBD)/math32/src

FINAL = $(SRCD)/main.c

TESTS = $(TESTD)/main.c
TESTS+= $(SUBD)/testoasterror/src/testoasterror.c

SRCS = $(SRCD)/rastarizer.c
SRCS+= $(SRCD)/circle.c
SRCS+= $(SRCD)/bezier.c
SRCS+= $(SRCD)/polygon.c
SRCS+= $(SUBD)/math32/src/math32.c

FINAL_OBJS:= $(patsubst %.c,$(OBJD)/%.o,$(FINAL))
SRCS_OBJS := $(patsubst %.c,$(OBJD)/%.o,$(SRCS))
TESTS_OBJS:= $(patsubst %.c,$(OBJD)/%.o,$(TESTS))

# aliases
.PHONY: final
final: $(BIND)/$(NAME)
tests: $(BIND)/tests

# generic compiling command
$(OBJD)/%.o: %.c
	@echo "building object $@"
	@mkdir -p $(@D)
	@$(CC) $(INCL) $(FLAGS) -c -o $@ $<

# final executable
$(BIND)/$(NAME): $(SRCS_OBJS) $(FINAL_OBJS)
	@echo "compiling executable $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LINK)

run:
	@cd $(BIND) && $(CMD)

img:
	@cd $(BIND) && sxiv test.ff

# tests executable
$(BIND)/tests: $(SRCS_OBJS) $(TESTS_OBJS)
	@echo "compiling tests"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LINK)

check:
	@cd $(BIND) && ./tests

# tools
leak: leakgrind
leakgrind: $(BIND)/$(NAME)
	@rm -f valgrind.log
	@cd $(BIND) && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	@less valgrind.log

leakcheck: leakgrindcheck
leakgrindcheck: $(BIND)/tests
	@rm -f valgrind.log
	@cd $(BIND) && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	@less valgrind.log

clean:
	@echo "cleaning"
	@rm -rf $(BIND) $(OBJD) valgrind.log

github:
	@echo "sourcing submodules from https://github.com"
	@cp .github .gitmodules
	@git submodule sync
	@git submodule update --init --remote

gitea:
	@echo "sourcing submodules from personal server"
	@cp .gitea .gitmodules
	@git submodule sync
	@git submodule update --init --remote
