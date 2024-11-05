all: transport.bin

transport.bin: transport.cpp
	g++ -std=c++20 -Ofast transport.cpp -o transport.bin

clean:
	rm $(wildcard *.bin)

PHONY: clean
