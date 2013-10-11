OBJ = learning.o main.o
HEADERS = learning.h

CC = g++
CFLAGS = -g
TGT = hmm

$(TGT): $(OBJ) bnc
	$(CC) $(OBJ) -o $(TGT)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $<

bnc:
	g++ -o bnc clean_bnc.cpp

clean:
	rm -f *.o hmm *.corp *.out bnc
