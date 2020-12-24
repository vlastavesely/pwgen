#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define FLAG_LOWER 0x00000001
#define FLAG_UPPER 0x00000002
#define FLAG_DIGIT 0x00000004
#define FLAG_ASCII 0x00000008

static void fatal(const char *fmt, ...)
{
	unsigned char msg[1024];
	va_list params;

	va_start(params, fmt);
	vsnprintf(msg, sizeof(msg), fmt, params);
	fprintf(stderr, "fatal: %s\n", msg);
	va_end(params);

	exit(-1);
}

int main(int argc, const char **argv)
{
	unsigned int i, j, n = 20, flags = 0;

	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];

		if (arg[0] == '-' && arg[1] == '-') {
			if (strcmp(arg, "--length") == 0) {
				arg = argv[++i];
				n = strtoul(arg, NULL, 10);
				continue;
			} else if (strcmp(arg, "--lower") == 0) {
				flags |= FLAG_LOWER;
				continue;
			} else if (strcmp(arg, "--upper") == 0) {
				flags |= FLAG_UPPER;
				continue;
			} else if (strcmp(arg, "--digit") == 0) {
				flags |= FLAG_DIGIT;
				continue;
			} else if (strcmp(arg, "--ascii") == 0) {
				flags |= FLAG_ASCII;
				continue;
			}
		}

		if (arg[0] == '-') {
			for (j = 1; arg[j]; j++) {
				switch (arg[j]) {
				case 'n':
					arg = argv[++i];
					n = strtoul(arg, NULL, 10);
					break;
				case 'l':
					flags |= FLAG_LOWER;
					break;
				case 'u':
					flags |= FLAG_UPPER;
					break;
				case 'd':
					flags |= FLAG_DIGIT;
					break;
				case 'a':
					flags |= FLAG_ASCII;
					break;
				default:
					fatal("unexpected parameter ‘%c’.", arg[j]);
				}
			}
			continue;
		}

		fatal("unexpected parameter ‘%s’.", arg);
	}

	printf("%08x\n", flags);

	return 0;
}
