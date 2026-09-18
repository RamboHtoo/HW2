all: tct

tct: TimeCode.cpp TimeCodeTests.cpp TimeCode.h
	g++ -Wall -O3 -g -std=c++17 TimeCode.cpp TimeCodeTests.cpp -o tct

clean:
	rm -f tct