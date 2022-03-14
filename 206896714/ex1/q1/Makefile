CFLAGS = -Wall
CC = gcc -std=c99
ECHO = echo "going to compile for target $@"
PROG = ex1_q1
FILE_NAME = merged.txt
TMP_FILE = tmp_file.txt

all: $(PROG) test

$(PROG).o: $(PROG).c
	@$(ECHO)
	$(CC) $(CFLAGS) -c $(PROG).c -o $(PROG).o

$(PROG): $(PROG).o
	$(CC) $(CFLAGS) $(PROG).o -o $(PROG)

test: $(PROG)
	touch merged.txt
	./$(PROG) grades_1.txt grades_2.txt

clean:
	rm -rf $(PROG) *.o $(FILE_NAME) $(TMP_FILE)