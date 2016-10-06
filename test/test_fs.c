#include <stdio.h>
#include <f_store.h>

int main(void)
{
	char *file_name = "/home/mialso/projects/file_store/bin/user.fstr";
	char *bad_file_name = "/home/mialso/projects/file_store/bin/users.fstr";
	char *not_file_name = "/home/mialso/projects/file_store/bin/users";
	int result = 0;
	struct F_store user_store = {0};

	if (0 != (result = init_store(file_name, &user_store))) {
		printf("good file test failed\n");
	} else {
		printf("good file test success\n");
	}
	if (1 != (result = init_store(bad_file_name, &user_store))) {
		printf("bad file test failed\n");
	} else {
		printf("bad file test OK\n");
	}
	if (3 != (result = init_store(not_file_name, &user_store))) {
		printf("not file name failed, res =%d\n", result);
	} else {
		printf("not file name test OK\n");
	}
	return 0;
}
