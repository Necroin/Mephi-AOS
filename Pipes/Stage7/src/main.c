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
    int file_desc, n;
	int buf_size = 80;
    char buf[buf_size];
	
	int pid = fork(); 

	if (pid == -1) {
		printf("Fork failed");
	}

	if (pid == 0) {
		file_desc = open(argv[1], O_WRONLY);
		if (file_desc == -1) {
			printf("Open file failed");
			return 0;
		}
		printf("\nChild started writing to file\n");
		const char message1[] = "Message";
		const char message2[] = " from ";
		const char message3[] = "Child\n";

		for(int i = 0; i < 10; ++i){
			write(file_desc, (void*)message1, strlen(message1));
			write(file_desc, (void*)message2, strlen(message2));
			write(file_desc, (void*)message3, strlen(message3));
		}
		return 0;
	}

	file_desc = open(argv[1], O_RDONLY);
	if (file_desc == -1) {
		printf("Open file failed");
		return 0;
	}

	sleep(1);

	printf("\nParent starting read from file\n");
    while ((n  = read(file_desc, buf, buf_size)) != 0) {
		write(1, buf, buf_size);
	}
	printf("\nParent end read from file\n");
	
	wait(NULL);
	close(file_desc);
    return 0;
}