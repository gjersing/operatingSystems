// CS 370
// mmap() example 1

#include <stdio.h>
#include <sys/mman.h>

int main()
{
	int	SIZE = 10;	// number of elements for the array   

	int	*ptr = mmap (NULL, SIZE*sizeof(int),
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				0, 0);

	if (ptr == MAP_FAILED){
		printf("Mapping Failed\n");
		return	1;
	}

	// fill the elements of the array
	for (int i=0; i < SIZE; i++)
		ptr[i] = i+10;

	for (int i=0; i < SIZE; i++)
		printf("[%d] ", ptr[i]);

	printf("\n");
	int	err = munmap(ptr, SIZE*sizeof(int));

	if (err != 0) {
		printf("UnMapping Failed\n");
		return 1;
	}

	return 0;
}

