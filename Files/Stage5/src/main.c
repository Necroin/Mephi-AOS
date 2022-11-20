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
void print_file(int file_descriptor);


int main(int argc, char* argv[]) {
	int file_descriptor = -2;
	int close_file_result = -2;
	if (!validate_command_line(argc, argv)) {
		return 0;
	}

	char* file_name = argv[1];
	char* access_rights = argv[2];

	int mode = parse_access_rights(access_rights);

	file_descriptor = open_file(file_name, O_CREAT | O_TRUNC | O_WRONLY, mode);
	printf("Open file result: %d\n", file_descriptor);
	if (file_descriptor == -1) {
		return 0;
	}

	const char* message = "Hello world\n";
    int message_len = strlen(message);
	for(int i =0; i < 5; ++i){
    	int write_result = write(file_descriptor, message, message_len);
		printf("Write file result: %d\n", write_result);
	}
	close_file_result = close(file_descriptor);
	printf("Close file result: %d\n", close_file_result);


	file_descriptor = open_file(file_name, O_RDONLY, mode);
	printf("Open file result: %d\n", file_descriptor);
	if (file_descriptor == -1) {
		return 0;
	}
	print_file(file_descriptor);
	close_file_result = close(file_descriptor);
	printf("Close file result: %d\n", close_file_result);



	file_descriptor = open_file(file_name, O_RDWR, mode);
	printf("Open file result: %d\n", file_descriptor);
	if (file_descriptor == -1) {
		return 0;
	}
	srand(time(NULL));
	
	struct stat st;
	int fstat_result = fstat(file_descriptor, &st);
	printf("fstat result: %d\n", fstat_result);
	if (fstat_result != 0) {
		return 0;
	}
	printf("File size: %ld\n", st.st_size);
    
	for(int i = 0; i < 5; ++i){
		off_t byte_place;
		const int buf_size = 1;
    	char read_buf[buf_size];
		int rand_result = rand();
		byte_place = (off_t)rand_result % st.st_size;
		int lseek_result = lseek(file_descriptor, byte_place, SEEK_SET);
		printf("lseek result: %d\n", lseek_result);
      	int read_result = read(file_descriptor, (void*)read_buf, buf_size);\
		printf("Read file result: %d\n", read_result);
		rand_result = rand();
		byte_place = (off_t)rand_result % st.st_size;
		lseek_result = lseek(file_descriptor, byte_place, SEEK_SET);
		printf("lseek result: %d\n", lseek_result);
		int write_result = write(file_descriptor, read_buf, buf_size);
		printf("Write file result: %d\n", write_result);
	}

	close_file_result = close(file_descriptor);
	printf("\nClose file result: %d\n", close_file_result);

	
	file_descriptor = open_file(file_name, O_RDONLY, mode);
	printf("Open file result: %d\n", file_descriptor);
	if (file_descriptor == -1) {
		return 0;
	}
	print_file(file_descriptor);
	close_file_result = close(file_descriptor);
	printf("Close file result: %d\n", close_file_result);
	return 0;
}

void usage() {
	printf("usage: file_worker file access_rights\n");
}

void help() {
	usage();
	printf("\tfile - name of file\n");
	printf("\taccess_rights:\n");
	printf("\t\tr - read\n");
	printf("\t\tw - write\n");
	printf("\t\tx - execute\n");
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

int parse_access_rights(char* access_rights) {
	int mode = 0;

	for(size_t i = 0; i < strlen(access_rights); ++i){
		if (access_rights[i] == 'r') {
			mode = mode | S_IRUSR | S_IRGRP | S_IROTH;
		}
		if (access_rights[i] == 'w') {
			mode = mode | S_IWUSR | S_IWGRP | S_IWOTH;
		}
		if (access_rights[i] == 'x') {
			mode = mode | S_IXUSR | S_IXGRP | S_IXOTH;
		}
	}

	return mode;
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

void print_file(int file_descriptor) {
	const int buf_size = 80;
    char read_buf[buf_size + 1];
    int read_count = 0;

    printf("File content:\n");
    while((read_count = read(file_descriptor, (void*)read_buf, buf_size)) != 0) {
		read_buf[read_count] = '\0';
    	printf("%s", read_buf);
    } 
}
