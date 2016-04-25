all: build test
	
build: main
	
main: Main.c
	gcc Main.c -o main
	
test:
	./main