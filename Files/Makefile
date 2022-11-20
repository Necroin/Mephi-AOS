edit:
	vi $(STAGE)/src/main.c

build: 
	g++ -c -Wall $(STAGE)/src/main.c -std=c++17
	g++ main.o -o $(STAGE)/files_worker
	rm *.o

test:
	$(STAGE)/test ./$(STAGE)/


task:
	cat ./task.txt
