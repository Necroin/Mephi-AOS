edit:
	vi Stage$(N)/src/main.c

build: 
	g++ -c -Wall Stage$(N)/src/main.c -std=c++17
	g++ main.o -o Stage$(N)/processes_worker
	rm *.o

test:
	Stage$(N)/test ./Stage$(N)/


task:
	cat ./task.txt
