#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


extern int errno;


void usage();
void help();
bool validate_command_line(int argc, char* argv[]);
int  open_file(char* file_name, int flags, int mode);

int main(int argc, char* argv[]){
	if (!validate_command_line(argc, argv)) {
		return 0;
	}

	char* file_name = argv[1];

	int file_descriptor = open_file(file_name, 0, 0);
	if (file_descriptor == -1) {
		return 0;
	}
	close(file_descriptor);
	return 0;
}

void usage() {
	printf("usage: file_worker file\n");
}

void help() {
	usage();
}

bool validate_command_line(int argc, char* argv[]){
	int command_line_arguments_count = 2;

	if (argc == 2 && strcmp(argv[1], "--help") == 0) {
		help();
		return false;
	}

	if (argc != command_line_arguments_count) {
		usage();
		return false;
	}

	return true;
}

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