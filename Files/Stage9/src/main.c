#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

extern int errno;


void usage();
void help();
bool validate_command_line(int argc, char* argv[]);
int  parse_access_rights(char* access_rights);
int  open_file(char* file_name, int flags, int mode);


int main(int argc, char* argv[]) {
	if (!validate_command_line(argc, argv)) {
		return 0;
	}

	int input_descriptor = 0;
	int output_descriptor = 1;
	char* from_file_name = argv[1];
	char* to_file_name = argv[2];

	int from_file_descriptor = open_file(from_file_name, O_RDONLY, 0);
	if (from_file_descriptor == -1) {
		return 0;
	}

	int to_file_descriptor = open_file(to_file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
	if (to_file_descriptor == -1) {
		return 0;
	}

	if(dup2(from_file_descriptor, input_descriptor) == -1) {
		perror("From file replace by input filed");
		return 0;
	}

	if(dup2(to_file_descriptor, output_descriptor) == -1) {
		perror("To file replace by output filed");
		return 0;
	}

	close(from_file_descriptor);
	close(to_file_descriptor);
	
	const int buf_size = 1;
    char read_buf[buf_size];
    int read_count = 0;

	while((read_count = read(input_descriptor, (void*)read_buf, buf_size)) != 0) {
    	write(output_descriptor, read_buf, buf_size);
    }

	return 0;
}

void usage() {
	printf("usage: file_worker <from file> <to file>\n");
}

void help() {
	usage();
}

bool validate_command_line(int argc, char* argv[]){
	int command_line_arguments_count = 3;

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
