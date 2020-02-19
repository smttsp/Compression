.PHONY: all run run2 run3

TARGET = main.o
SRC = main.cpp

$(TARGET) : $(SRC)
	g++ -std=c++14 $(SRC) -o $(TARGET)

all: run run2 run3

run:
	./main.o im_test2.jpg 0

run2:
	./main.o im_test2.jpg.mzip 1

run3:
	./main.o im_test2.jpg im_test2.jpg.mzip.txt 2

clean:
	rm *.o
