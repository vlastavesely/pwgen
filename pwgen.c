#include <stdio.h>

int main(int argc, const char **argv)
{
	unsigned int i;

	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];
		printf("%s\n", arg);
	}

	return 0;
}
