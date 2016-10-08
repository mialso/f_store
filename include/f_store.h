#ifndef MLS_F_STORE_H
#define MLS_F_STORE_H 

#include <stdint.h>

#define NAME_SIZE 32
#define KEY_SIZE 32
struct F_item {
	uint64_t id;
	char name[NAME_SIZE]; 	// 4 x 64 (8 x 8)
	char key[KEY_SIZE];	// 4 x 64 (8 x 8)
	uint64_t role;
};
struct F_range {
	struct F_range *next;
	uint16_t start;
	uint16_t end;
};
struct F_store {
	uint64_t size;
	char name[8];
	struct F_item *data;
	struct F_range *free;
};

struct F_item *init_item();
int init_store(char *name, struct F_store *store);
uint64_t add_instance(struct F_store *store, struct F_item *item);
struct F_item *get_item(struct F_store *store, uint64_t num);

uint64_t update_item(struct F_store *store, struct F_item *item, uint64_t s_id);
uint64_t delete_item(struct F_store *store, uint64_t num);

#endif
