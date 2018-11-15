OBJECT = good_echo.o
ASSEMBLY = good_echo.s
PROGRAM = good_echo
GCC = gcc
CFLAGS = -O1 -Wall -fstack-protector

all: good_echo good_echo.s good_echo.o

good_echo: good_echo.c
	$(GCC) -Wall -Wstack-protector -O1 -fstack-protector -o $@ $<
good_echo.s: good_echo.c
	$(GCC) $(CFLAGS) -S $<
good_echo.o: good_echo.c
	 $(GCC) $(CFLAGS) -c $<
clean:
	rm -f $(OBJECT) $(ASSEMBLY) good_echo
