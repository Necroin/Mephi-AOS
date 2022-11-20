#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int buffer_size = 80;
	char buffer[buffer_size + 1];

	int pid = fork(); 

	if (pid == -1) {
		printf("Fork failed");
	}

	if (pid == 0) {
		while(true) {
			int read_count = read(0, buffer, buffer_size);
			printf("Child:\n");
			write(1, buffer, read_count);
		}
		return 0;
	}

	while(true) {
		int read_count = read(0, buffer, buffer_size);
		printf("Parent:\n");
		write(1, buffer, read_count);
	}
	wait(NULL);
    return 0;
}