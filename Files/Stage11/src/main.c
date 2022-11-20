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


int main(int argc, char* argv[]) {
	if (!validate_command_line(argc, argv)) {
		return 0;
	}
	
	long max_size = 0;
	char* max_file_name = "";
	for (int i = 1; i < argc; ++i) {
		struct stat file_stat;
		char* file_name = argv[i];
		int fstat_result = stat(file_name, &file_stat);
		if (fstat_result != 0) {
			printf("File not exist: %s\n", file_name);
			continue;
		}

		long file_size = file_stat.st_size;
		if (file_size >= max_size) {
			max_size = file_size;
			max_file_name = file_name;
		}
	}

	printf("File name: %s\n", max_file_name);
	printf("Total size, in bytes: %ld\n", max_size);

	return 0;
}

void usage() {
	printf("usage: file_worker <file> [<file> ...]\n");
}

void help() {
	usage();
}

bool validate_command_line(int argc, char *argv[]) {
	if (argc == 2 && strcmp(argv[1], "--help") == 0) {
		help();
		return false;
	}

	if (argc == 1) {
		usage();
		return false;
	}

	return true;
}
