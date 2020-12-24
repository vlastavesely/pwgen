#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define FLAG_LOWER 0x00000001
#define FLAG_UPPER 0x00000002
#define FLAG_DIGIT 0x00000004
#define FLAG_ASCII 0x00000008

#define PWGEN_DEFAULT_PASSLEN 20

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

static inline bool is_ascii(unsigned int c)
{
	return c > ' ' && c < 0x7f;
}

static inline bool is_digit(unsigned int c)
{
	return c >= '0' && c <= '9';
}

static inline bool is_lower(unsigned int c)
{
	return c >= 'a' && c <= 'z';
}

static inline bool is_upper(unsigned int c)
{
	return c >= 'A' && c <= 'Z';
}

static inline bool is_accepted(unsigned int c, unsigned int flags)
{
	return ((flags & FLAG_LOWER) && is_lower(c)) ||
	       ((flags & FLAG_UPPER) && is_upper(c)) ||
	       ((flags & FLAG_DIGIT) && is_digit(c)) ||
	       ((flags & FLAG_ASCII) && is_ascii(c));
}

static int randomise(unsigned char *buf, unsigned int len)
{
	int ret, n, fd;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
		return -errno;
	n = read(fd, buf, len);
	ret = (n != len) ? -errno : 0;
	close(fd);

	return ret;
}

static int generate_password(unsigned int flags, unsigned int n)
{
	unsigned char buf[10], res[n];
	int ret, i, pos = 0;

	while (1) {
		ret = randomise(buf, sizeof(buf));
		if (ret != 0)
			return ret;

		for (i = 0; i < sizeof(buf); i++) {
			unsigned int c = buf[i];
			if (is_accepted(c, flags)) {
				res[pos++] = c;
				if (pos == n)
					goto out;
			}
		}
	}
out:
	printf("%s\n", res);
	memset(res, '\0', n);
	return 0;
}

int main(int argc, const char **argv)
{
	unsigned int i, j, flags = 0;
	unsigned int n = PWGEN_DEFAULT_PASSLEN;
	int ret;

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

	ret = generate_password(flags, n);

	return ret;
}
