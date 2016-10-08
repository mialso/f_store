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

static int add_free_pointer(struct F_store *store, uint16_t num);
static int init_free_pointer(struct F_store *store);
static void remove_free_pointer(struct F_store *store, uint64_t num);

static inline void copy_item_data(struct F_item *to, struct F_item *from);
static inline int validate_item_fail(struct F_item *item);
/*
 * purpose: to init store with mapped file to memory
 * + map memory to file
 * + init store map structure
 */
int init_store(char *name, struct F_store *store)
{
	struct stat sb;
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
int add_free_pointer(struct F_store *store, uint16_t num)
{
	struct F_range *range = store->free;
	struct F_range *prev_range = NULL;
	struct F_range *next_range = NULL;
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
				next_range = range;
				range = calloc(1, sizeof(struct F_range));
				if (NULL == range) {
					fprintf(stderr, "free add memory error\n");
					return 1;
				}
				range->start = num;
				range->end = num;
				range->next = next_range;
				if (store->free == next_range) {
					store->free = range;
				} else {
					prev_range->next = range;
				}
			}
			return 0;
		} else if (num > range->end) {
			if (1 == (num - range->end)) {
				++range->end;
				return 0;
			} else {
				prev_range = range;
				range = range->next;
			}
		} else {
			fprintf(stderr, "free add loop error: already free, num = %d;\n", num);
			return 1;
		}
	}
	// no way to get here
	fprintf(stderr, "im on a wrong way: num =%lu\n", (uint64_t) num);
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
		res = add_free_pointer(store, counter);
		if (res) return res;
	}
	return res;
}	
/*
 * pupose: to remove pointer from free list
 * the logic is to remove only the firts free pointer,
 * because only first would be inserted
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
	++range->start;
}
/*
 * purpose: to add instance to store
 */
uint64_t add_instance(struct F_store *store, struct F_item *item)
{
	if (validate_item_fail(item)) {
		return 0;
	}
	uint64_t item_place = store->free->start;
	struct F_item *new_item = store->data+item_place;
	copy_item_data(new_item, item);
	remove_free_pointer(store, item_place);
	return item_place+1;
}
int validate_item_fail(struct F_item *item)
{
	if ('\0' != item->name[NAME_SIZE-1] || '\0' != item->key[KEY_SIZE-1]) {
		fprintf(stderr, "not valid item {%zu} provided\n", item->id);
		return 1;
	}
	return 0;
}
void copy_item_data(struct F_item *to, struct F_item *from)
{
	strncpy(to->name, from->name, NAME_SIZE-1);
	strncpy(to->key, from->key, KEY_SIZE-1);
	to->id = from->id;
	to->role = from->role;
}
struct F_item *get_item(struct F_store *store, uint64_t s_id)
{
	uint16_t num = 0;
	if (0 == s_id  || FS_SIZE < s_id) {
		printf("get_item: [%zu] is out of store capacity\n", s_id);
		return NULL;
	}
	num = s_id-1;
	struct F_item *item = store->data+num;
	if (item->id) {
		return item;
	} else {
		return NULL;
	}
}
/*
 * purpose: to update item
 */
uint64_t update_item(struct F_store *store, struct F_item *item, uint64_t s_id)
{
	if (validate_item_fail(item)) {
		return 1;
	}
	struct F_item *store_item = get_item(store, s_id);
	if (NULL == store_item) {
		fprintf(stderr, "update_item(): item [%zu] not found\n", s_id);
		return 2;
	}
	copy_item_data(store_item, item);
	return 0;
}
/*
 * purpose: to delete item
 */
uint64_t delete_item(struct F_store *store, uint64_t s_id)
{
	if (store->size < s_id || 0 == s_id) {
		fprintf(stderr, "delete item: wrong s_id [%lu] provided\n", s_id);
		return 255;
	}
	uint16_t num = s_id-1;
	memset(store->data+num, 0, sizeof(struct F_item));
	add_free_pointer(store, num);
	return 0;
}
