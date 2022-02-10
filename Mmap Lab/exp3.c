// CS 370
// mmap() example 3

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

	if (argc < 2) {
		printf("File path not mentioned\n");
		exit(0);
	}

	const char *filepath = argv[1];
	int	fd = open(filepath, O_RDWR);

	if (fd < 0) {
		printf("\n\"%s \" could not open\n", filepath);
		exit(1);
	}

	struct	stat statbuf;
	int	err = fstat(fd, &statbuf);

	if (err < 0){
		printf("\n\"%s \" could not open\n", filepath);
		exit(2);
	}

	char *ptr = mmap(NULL,statbuf.st_size,
			PROT_READ|PROT_WRITE, MAP_SHARED, fd,0);

	if (ptr == MAP_FAILED) {
		printf("Mapping Failed\n");
		return 1;
	}

	// write contents of file to console
	printf("Original File\n");
	ssize_t n = write(1,ptr,statbuf.st_size);
	if (n != statbuf.st_size)
		printf("Write failed\n");

	// reverse the file contents
	char	tmp;
	int	j = statbuf.st_size-2;
	for (int i=0; i < (statbuf.st_size-1)/2; i++) {
		tmp = ptr[i];
		ptr[i] = ptr[j];
		ptr[j] = tmp;
		j--;
	}

	if (msync(ptr, statbuf.st_size, MS_SYNC) == -1) {
		printf("Could not sync the file to disk");
		return	1;
	}

	// write contents of file to console
	printf("\nBackwards File\n");
	n = write(1, ptr, statbuf.st_size);
	if (n != statbuf.st_size)
		printf("Write failed\n");

	err = munmap(ptr, statbuf.st_size);

	if (err != 0) {
		printf("UnMapping Failed\n");
		return 1;
	}


	close(fd);
	return 0;
}

