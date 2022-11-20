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
int  open_file(char* file_name, int flags, int mode);


int main(int argc, char* argv[]) {

	int file_descriptor = -2;
	int close_file_result = -2;
	if (!validate_command_line(argc, argv)) {
		return 0;
	}

	char* file_name = argv[1];

	file_descriptor = open_file(file_name, O_RDONLY, 0);
	printf("Open file result: %d\n", file_descriptor);
	if (file_descriptor == -1) {
		return 0;
	}
	
	struct stat file_stat;
	int fstat_result = fstat(file_descriptor, &file_stat);
	printf("fstat result: %d\n", fstat_result);
	if (fstat_result != 0) {
		return 0;
	}

	printf("ID of device containing file: %ld\n", file_stat.st_dev);
	printf("inode number: %ld\n", file_stat.st_ino);
	printf("protection: %d\n", file_stat.st_mode);
	printf("number of hard links: %ld\n", file_stat.st_nlink);
	printf("user ID of owner: %d\n", file_stat.st_uid);
	printf("group ID of owner: %d\n", file_stat.st_gid);
	printf("device ID (if special file): %ld\n", file_stat.st_rdev);
	printf("total size, in bytes: %ld\n", file_stat.st_size);
	printf("blocksize for file system I/O: %ld\n", file_stat.st_blksize);
	printf("number of 512B blocks allocated: %ld\n", file_stat.st_blocks);
	printf("time of last access: %ld\n", file_stat.st_atime);
	printf("time of last modification: %ld\n", file_stat.st_mtime);
	printf("time of last status change: %ld\n", file_stat.st_ctime);

	close_file_result = close(file_descriptor);
	printf("\nClose file result: %d\n", close_file_result);
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
