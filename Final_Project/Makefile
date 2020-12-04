all: parse

parser.cpp parser.hpp: parser.y
	bison -d -o parser.cpp parser.y

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l

parse: main.cpp ast.cpp parser.cpp scanner.cpp
	g++ -std=c++0x main.cpp ast.cpp parser.cpp scanner.cpp `llvm-config-7.0-64 --cppflags --ldflags --libs --system-libs all` -o parse

output.o: parse
					./parse

test: test.cpp output.o
		g++ test.cpp output.o -o test

output.o: test
				./test
clean:
	rm -f parse scanner.cpp parser.cpp parser.hpp test *.o
