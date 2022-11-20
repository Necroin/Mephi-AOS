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

	struct flock rlock, wlock;

	rlock.l_type = F_RDLCK;
	rlock.l_whence = SEEK_CUR;
	rlock.l_start = 0;
	rlock.l_len = 0;

	wlock.l_type = F_WRLCK;
	wlock.l_whence = SEEK_CUR;
	wlock.l_start = 0;
	wlock.l_len = 0;

	int pid = fork(); 

	if (pid == -1) {
		printf("Fork failed");
	}

	if (pid == 0) {
		while(true) {
			rlock.l_type = F_RDLCK;
			wlock.l_type = F_WRLCK;
			int rlock_res = fcntl(0, F_SETLKW, &rlock);
			int wlock_res = fcntl(0, F_SETLKW, &wlock);
			int read_count = read(0, buffer, buffer_size);
			if (read_count > 0) {
				rlock.l_type = F_UNLCK;
				wlock.l_type = F_UNLCK;
				fcntl(0, F_SETLK, &rlock);
				fcntl(1, F_SETLK, &wlock);
				sleep(1);
			}
			printf("Child:\n\t");
			write(1, buffer, read_count);
		}
		return 0;
	}

	while(true) {
		while(true) {
			rlock.l_type = F_RDLCK;
			wlock.l_type = F_WRLCK;
			int rlock_res = fcntl(0, F_SETLKW, &rlock);
			int wlock_res = fcntl(0, F_SETLKW, &wlock);
			int read_count = read(0, buffer, buffer_size);
			if (read_count > 0) {
				rlock.l_type = F_UNLCK;
				wlock.l_type = F_UNLCK;
				fcntl(0,F_SETLK, &rlock);
				fcntl(1,F_SETLK, &wlock);
				sleep(1);
			}
			printf("Parent:\n\t");
			write(1, buffer, read_count);
		}
	}
	wait(NULL);
    return 0;
}