# # # # # # # # # # # # # # # # #
# These files need to be edited #
# # # # # # # # # # # # # # # # #

# Name of static library
SLIB = s21_decimal.a

# Debug flags if needed
DFLAGS = -std=c11

# Flags for code analysis
CFLAGS = -Wall -Werror -Wextra

# # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # #

# Compilation variables
CC = gcc
CC += $(DFLAGS)
CC += $(CFLAGS)
CFILES = $(wildcard s21_*.c service_*.c)
OBJ = $(CFILES:.c=.o)

# Check framework library flags
TMAIN = s21_decimal_test.c
TEXEC = $(TMAIN:.c=)
TFLAGS = -lcheck

OS = $(shell uname)
ifeq ($(OS), Linux)
TFLAGS += -lm -lpthread -lrt -lsubunit
endif

# GCOV report flags
GFLAGS = --coverage
GSLIB = gcov_$(SLIB)
GTEXEC = gcov_$(TEXEC)
RFOLDER = report
RNAME = $(SLIB:.a=_report)

all: build_test gcov_report

build: test

rebuild: clean test

static_checks:
	cp ../materials/linters/CPPLINT.cfg ../
	python3 ../materials/linters/cpplint.py --extensions=c *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

ifeq ($(OS), Darwin)
LEAK_CHECK = leaks --atExit -q -- 
endif
ifeq ($(OS), Linux)
LEAK_CHECK = valgrind --leak-check=full -s -q -- 
endif

leak_checks: build_test
	$(LEAK_CHECK)./$(TEXEC)

check: static_checks leak_checks

$(SLIB): $(OBJ)
	ar -rcs $(SLIB) $(OBJ)

$(OBJ): $(CFILES)
	$(CC) -c $(CFILES)

build_test: $(SLIB)
	$(CC) $(TMAIN) $(SLIB) -o $(TEXEC) $(TFLAGS)

test: build_test
	./$(TEXEC)

gcov_report: $(GSLIB)
	$(CC) $(TMAIN) $(GSLIB) -o $(GTEXEC) $(TFLAGS) $(GFLAGS)
	./$(GTEXEC)
	lcov -t "$(RNAME)" -o $(RNAME).info -c -d .
	genhtml -o $(RFOLDER) $(RNAME).info

$(GSLIB): gcov_obj
	ar -rcs $(GSLIB) $(OBJ)

gcov_obj: $(CFILES)
	$(CC) -c $(CFILES) $(GFLAGS)

clean:
	rm -rf *.o *.a *.dSYM
	rm -rf report
	rm -rf *.out *.gch
	rm -rf *.gcda *.gcno *.info
	rm -rf $(TEXEC) $(GTEXEC)