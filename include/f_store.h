#ifndef MLS_F_STORE_H
#define MLS_F_STORE_H 

#include <stdint.h>

struct F_instance {
	char *name;
	char *key;
	uint64_t id;
};
struct F_map {
	uint64_t *data;
	uint64_t size;
};
struct F_store {
	struct F_map map;
	uint64_t size;
	void (*get_instance)(struct F_instance *item);
	void (*update_instance)(struct F_instance *item);
	void (*add_instance)(struct F_instance *item);
};

struct F_instance *init_item();
int init_store(char *name, struct F_store *store);

#endif
