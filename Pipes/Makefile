edit:
	vi Stage$(N)/src/main.c

build: 
	g++ -c -Wall Stage$(N)/src/main.c -std=c++17
	g++ main.o -o Stage$(N)/pipes_worker
	rm *.o

test:
	rm -rf tmp/*
	Stage$(N)/test ./Stage$(N)/


task:
	cat ./task.txt
