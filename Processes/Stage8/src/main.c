#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int open_file(char* file_name, int flags, int mode) {
	int fd = open(file_name, flags, mode);
	if (fd == -1) {
		printf("Open file error code: %d\n", errno);
		perror(strerror(errno));
		return fd;
	}
	printf("Opened file: %s\n", file_name);
	return fd;
}

int main(int argc, char* argv[]) {
    const char* process = "Unknown";
    char* file_name = argv[1];
    int file_descriptor = open_file(file_name, O_RDONLY, 0);
	if (file_descriptor == -1) {
		return 0;
	}

    const int buf_size = 10;
    char read_buf[buf_size + 1];
    int read_count = 0;

    pid_t pid = fork();  

    if (pid == -1) {
        printf("Fork error");
    }

    if (pid == 0) {
        process = "Child";
        printf("%s: start working\n", process);

        int child_file_descriptor = open_file("child_data_file", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
	    if (child_file_descriptor == -1) {
		    return 0;
	    }

        while((read_count = read(file_descriptor, (void*)read_buf, buf_size)) != 0) {
		    read_buf[read_count] = '\0';
            write(child_file_descriptor, read_buf, read_count);
        }

        close(child_file_descriptor);
    }

    if (pid > 0) {
        process = "Parent";
        printf("%s start working\n", process);

        int parent_file_descriptor = open_file("parent_data_file", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
	    if (parent_file_descriptor == -1) {
		    return 0;
	    }

        while((read_count = read(file_descriptor, (void*)read_buf, buf_size)) != 0) {
		    read_buf[read_count] = '\0';
            write(parent_file_descriptor, read_buf, read_count);
        }
        
        close(parent_file_descriptor);

        int status;
        pid_t child_pid = wait(&status);
        close(file_descriptor);
        printf("%s: terminated child pid=%d\n", process, child_pid);
    }

    return 0;
}
