#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <f_store.h>

#define FS_SIZE 80000

static inline int init_store_data(struct F_store *store, void *data);
static int free_add(struct F_store *store, uint16_t num);
static int init_free_pointer(struct F_store *store);
static void remove_free_pointer(struct F_store *store, uint64_t num);
/*
 * purpose: to init store with mapped file to memory
 * + map memory to file
 * + init store map structure
 */
int init_store(char *name, struct F_store *store)
{
	struct stat sb;
	//off_t len;
	char *p;
	int fd;

	if ('\0' == store->name[0] || '\0' != store->name[7]) {
		fprintf(stderr, "not valid store name provided\n");
		return 1;
	}
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
	if (FS_SIZE != sb.st_size) {
		fprintf(stderr, "%s file is not of %d size\n", name, FS_SIZE);
		return 4;
	}
	p = mmap (NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == p) {
		perror("f_store mmap");
		return 5;
	}
	if (-1 == close(fd)) {
		perror("f_store close");
		return 6;
	}
	if (init_store_data(store, p)) {
		fprintf(stderr, "store init error\n");
		return 7;
	}
	return 0;
}
/*
 * purpose: init store; possible inline
 */
int init_store_data(struct F_store *store, void *data)
{
	int res = 0;
	store->data = (struct F_item *)data;
	store->size = FS_SIZE/sizeof(struct F_item);
	res = init_free_pointer(store);
	return res;
}
/*
 * purpose: add free pointer to free list
 */
int free_add(struct F_store *store, uint16_t num)
{
	struct F_range *range = store->free;
	if (NULL == range) {
		range = calloc(1, sizeof(struct F_range));
		if (NULL == range) {
			fprintf(stderr, "free add memory error\n");
			return 1;
		}
		range->start = store->size;
		range->end = store->size;
		store->free = range;
	}
	while (range) {
		if (num < range->start) {
			if (1 == (range->start - num)) {
				--range->start;
			} else {
				range = calloc(1, sizeof(struct F_range));
				if (NULL == range) {
					fprintf(stderr, "free add memory error\n");
					return 1;
				}
				range->start = num;
				range->end = num;
				range->next = store->free;
				store->free = range;
			}
			return 0;
		} else if (num > range->end) {
			if (1 == (num - range->end)) {
				++range->end;
				return 0;
			} else {
				range = range->next;
			}
		} else {
			fprintf(stderr, "free add loop error, num = %d;\n", num);
			return 1;
		}
	}
	// no way to get here
	fprintf(stderr, "im on a wrong way\n");
	return 2;
}
/*
 * purpose: populate free listing
 */
int init_free_pointer(struct F_store *store)
{
	uint16_t counter = store->size;
	int res = 0;
	while (0 < counter) {
		--counter;
		if ((store->data+counter)->id) {
			continue;
		}
		res = free_add(store, counter);
		if (res) return res;
	}
	return res;
}	
/*
 * pupose: to remove pointer from free list
 */
void remove_free_pointer(struct F_store *store, uint64_t num)
{
	struct F_range *range = store->free;
	if (num != range->start) {
		// this logic is not supported
		fprintf(stderr, "remove_free_pointer(): unsopported logic\n");
		return;
	}
	if (range->start == range->end) {
		store->free = range->next;
		free(range);
		return;
	}
	--range->start;
}
/*
 * purpose: to add instance to store
 */
uint64_t add_instance(struct F_store *store, struct F_item *item)
{
	if ('\0' != item->name[NAME_SIZE-1] || '\0' != item->key[KEY_SIZE-1]) {
		fprintf(stderr, "not valid instance provided to <%s> store\n", store->name);
		return 0;
	}
	uint64_t item_place = store->free->start;
	struct F_item *new_item = store->data+item_place;
	strncpy(new_item->name, item->name, NAME_SIZE-1);
	strncpy(new_item->key, item->key, KEY_SIZE-1);
	new_item->id = item_place+1;
	new_item->role = item->role;
	remove_free_pointer(store, item_place);
	return new_item->id;
}
struct F_item *get_item(struct F_store *store, uint64_t num)
{
	if (0 == num  || FS_SIZE < num) {
		printf("get_item: [%zu] is out of store capacity\n", num);
		return NULL;
	}
	struct F_item *item = store->data+num-1;
	if (item->id) {
		return item;
	} else {
		return NULL;
	}
}
/*
 * purpose: to update item
 */
uint64_t update_item(struct F_store *store, struct F_item *item)
{
	return 1;
}
/*
 * purpose: to update item
 */
uint64_t delete_item(struct F_store *store, uint64_t num)
{
	memset(store->data+num-1, 0, sizeof(struct F_item));
	free_add(store, num-1);
	return 1;
}
