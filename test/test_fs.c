#include <stdio.h>
#include <f_store.h>
#include <string.h>

#define ONE_SIZE_DATA "se5en"
#define TWO_SIZE_DATA "sev9nteen"
#define FOUR_SIZE_DATA "some long 25 symbols line"
#define WRONG_SIZE_DATA "some very long 51 symbols line, that is not allowed"

#define STOR_SIZE 1000

static void print_free_list(struct F_store *store);
static void print_item(struct F_item *item);
static int verify_item(struct F_store *store, struct F_item *item, uint64_t num);

int main(void)
{
	char *file_name = "/home/mialso/projects/file_store/bin/user.fstr";
	char *bad_file_name = "/home/mialso/projects/file_store/bin/users.fstr";
	char *not_file_name = "/home/mialso/projects/file_store/bin/users";
	int result = 0;
	struct F_store store = {0};

	uint16_t new_item_id = 0;

	strcpy(store.name, "user");
	if (0 != (result = init_store(file_name, &store))) {
		printf("good file test [FAIL]\n");
	} else {
		printf("good file test [OK]\n");
	}
	if (1 != (result = init_store(bad_file_name, &store))) {
		printf("bad file test [FAIL]\n");
	} else {
		printf("bad file test [OK]\n");
	}
	if (3 != (result = init_store(not_file_name, &store))) {
		printf("not file name, res =%d; [FAIL]\n", result);
	} else {
		printf("not file name test [OK]\n");
	}
	if (STOR_SIZE == store.size) {
		printf("size = %zu test [OK]\n", store.size);
	} else {
		printf("size = %zu test [FAIL]\n", store.size);
	}
	// check listing
	print_free_list(&store);
	// test store operations
	struct F_item instance;
	strcpy(instance.name, "valid name");
	strcpy(instance.key, "valid key");
	instance.role = 1;
	if (0 == (new_item_id = add_instance(&store, &instance))) {
		printf("%s data add error\n", instance.name);
		instance.id = new_item_id;
	} else {
		printf("%s data added to %d: [OK]\n", instance.name, new_item_id);
		instance.id = new_item_id;
	}
	verify_item(&store, &instance, new_item_id);
	print_free_list(&store);
	strcpy(instance.name, "not valid very long name,aaaaa5aaaaa5aaaaa5aaaaa5");
	strcpy(instance.key, "not valid very logn key,aaaaa5aaaaa5aaaaa5aaaaa5");
	instance.role = 2;
	if (0 == (new_item_id = add_instance(&store, &instance))) {
		printf("%s data add error\n", instance.name);
		instance.id = new_item_id;
	} else {
		printf("%s data added to %d: [OK]\n", instance.name, new_item_id);
		instance.id = new_item_id;
	}
	verify_item(&store, &instance, new_item_id);
	print_item(&instance);
	print_item(get_item(&store, new_item_id));

	delete_item(&store, 4);
	print_free_list(&store);

	memset(instance.name, 0, 32);
	memset(instance.key, 0, 32);
	strcpy(instance.name, "new valid name");
	strcpy(instance.key, "new valid key");
	instance.role = 1;
	instance.id = 2;
	update_item(&store, &instance, 2);
	verify_item(&store, &instance, 2);
	
	memset(instance.name, 0, 32);
	memset(instance.key, 0, 32);
	strcpy(instance.name, "another valid name");
	strcpy(instance.key, "another valid key");
	instance.role = 1;
	instance.id = 1343422;
	print_item(&instance);
	add_instance(&store, &instance);

	delete_item(&store, 8);
	print_free_list(&store);

	return 0;
}
void print_free_list(struct F_store *store)
{
	struct F_range *range = store->free;
	while (range) {
		printf("range = [%d,%d]\n", range->start, range->end);
		range = range->next;
	}
}
int verify_item(struct F_store *store, struct F_item *item, uint64_t num)
{
	struct F_item *s_item = get_item(store, num);
	if (!s_item) {
		printf("item [%zu] not found\n", num);
		return 1;
	}
	if (strcmp(item->name, s_item->name)) {
		printf("verify item names are not equal\n");
		print_item(item);
		print_item(s_item);
		return 2;
	}
	if (strcmp(item->key, s_item->key)) {
		printf("verify item keys are not equal\n");
		print_item(item);
		print_item(s_item);
		return 3;
	}
	if (item->id != s_item->id) {
		printf("verify item id's are not equal\n");
		print_item(item);
		print_item(s_item);
		return 3;
	}
	if (item->role != s_item->role) {
		printf("verify item roles are not equal\n");
		print_item(item);
		print_item(s_item);
		return 3;
	}
	return 0;
}
void print_item(struct F_item *item)
{
	if (NULL == item) {
		printf("NULL print item call\n");
		return;
	}
	printf("item: <%s> {%s} [%zu] r:%zu;\n",
				   item->name,
				   item->key,
				   item->id,
				   item->role
	);
}
