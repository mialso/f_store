#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <f_store.h>


/*
 * purpose: to init store with mapped file to memory
 */
int init_store(char *name, struct F_store *store)
{
	struct stat sb;
	off_t len;
	char *p;
	int fd;

	fd = open(name, O_RDWR);
	if (-1 == fd) {
		perror("f_store open");
		return 1;
	}
	if (-1 == fstat(fd, &sb)) {
		perror("f_store fstat");
		return 2;
	}
	// check file for mode
	if (!S_ISREG (sb.st_mode)) {
		fprintf(stderr, "%s is not a file\n", name);
		return 3;
	}
	p = mmap (NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == p) {
		perror("f_store mmap");
		return 4;
	}
	if (-1 == close(fd)) {
		perror("f_store close");
		return 5;
	}
	if (NULL == (store->map.data = (uint64_t *)p)) {
		fprintf(stderr, "store pointer is NULL\n");
		return 6;
	}
	// TODO calculate the size
	return 0;
}
