#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
int pipe_descriptors[2];

const int READ_PIPE = 0;
const int WRITE_PIPE = 1;

const char fifo_name[] = "tmp/pipes_worker_6";

int main(int argc, char* argv[]) {
    int fifo_desc, n;
    char buf[80] = "\0";

    int fifo = mkfifo(fifo_name, 0777);
    if (fifo < 0) {
		printf("FIFO creation failed\n");
		return 0;
	}

	int pid = fork(); 

	if (pid == -1) {
		printf("Fork failed");
	}

	int mode = O_RDONLY;
	if (pid == 0 ) { mode = O_WRONLY; sleep(2); }

    if ((fifo_desc = open(fifo_name, mode | O_NDELAY)) < 0) {
		printf("Fifo opening failed\n");
		return 1;
	}

	if (pid == 0) {
		printf("Child started\n");
		const char message[] = "Message from Child\n";
		write(fifo_desc, message, strlen(message));
		close(fifo_desc);
		return 0;
	}

	printf("Parent starting read from fifo\n");
	sleep(3);
    while ((n  = read(fifo_desc, buf, sizeof(buf)))) {
		buf[n] = 0;
		printf("%s", buf);
	}
	printf("Parent end read from fifo\n");
	
	wait(NULL);
	close(fifo_desc);
    unlink(fifo_name);
    return 0;
}