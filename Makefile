OBJ = learning.o main.o
HEADERS = learning.h

CC = g++
CFLAGS = -g
TGT = hmm

$(TGT): $(OBJ)
	$(CC) $(OBJ) -o $(TGT)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o hmm *.corp
	