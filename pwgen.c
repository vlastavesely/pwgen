#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>

#define PROGNAME "pwgen"
#define VERSION "1.0"

#define FLAG_LOWER 0x00000001
#define FLAG_UPPER 0x00000002
#define FLAG_DIGIT 0x00000004
#define FLAG_ASCII 0x00000008

#define DEFAULT_PASSLEN 20

static const char *user_charset = NULL;
static char *usage_str =
	"Usage: pwgen [OPTION]...\n"
	"Generate a secure password.\n"
	"\n"
	"  -l, --lower              Use lowercase letters.\n"
	"  -u, --upper              Use uppercase letters.\n"
	"  -d, --digit              Use digits.\n"
	"  -a, --ascii              Use all printable ASCII characters.\n"
	"  -n, --length [LENGTH]    Length of the generated password.\n"
	"  -r, --repeat [COUNT]     Generate multiple passwords.\n"
	"  -c, --charset [CHARSET]  User specified character set.\n"
	"  -v, --version            Print version number and exit.\n"
	"  -h, --help               Show this message and exit.\n";

static const char *short_opts = "hvludan:r:c:";
static const struct option long_opts[] = {
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'v'},
	{"lower",     no_argument,       0, 'l'},
	{"upper",     no_argument,       0, 'u'},
	{"digit",     no_argument,       0, 'd'},
	{"ascii",     no_argument,       0, 'a'},
	{"length",    required_argument, 0, 'n'},
	{"repeat",    required_argument, 0, 'r'},
	{"charset",   required_argument, 0, 'c'},
	{0, 0, 0, 0}
};

static void fatal(const char *fmt, ...)
{
	char msg[1024];
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
	unsigned int i;

	if (user_charset) {
		for (i = 0; i < strlen(user_charset); i++) {
			if (user_charset[i] == c) {
				return true;
			}
		}
	}

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
	unsigned char buf[128], res[n + 1];
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
	res[n] = '\0';
	printf("%s\n", res);
	memset(res, '\0', n);

	return 0;
}

static void print_usage()
{
	puts(usage_str);
}

static void print_version()
{
	printf("%s %s\n", PROGNAME, VERSION);
}

int main(int argc, const char **argv)
{
	unsigned int n = DEFAULT_PASSLEN, flags = 0;
	int opt_index = 0, c = 0, r = 1, ret;

	opterr = 0; /* disable the auto error message */
	while (c != -1) {
		c = getopt_long(argc, (char *const *) argv, short_opts,
				long_opts, &opt_index);
		switch (c) {
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
		case 'n':
			n = strtoul(optarg, NULL, 10);
			break;
		case 'r':
			r = strtoul(optarg, NULL, 10);
			break;
		case 'c':
			user_charset = optarg;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
			print_usage();
			exit(0);
			break;
		case '?':
			fatal("unrecognised option '-%s'.", optopt ?
				(char *) &(optopt) : argv[optind - 1] + 1);
			return -1;
		default:
			break;
		}
	}

	if (flags == 0 && user_charset == NULL) {
		print_usage();
		exit(1);
	}

	while (r--) {
		ret = generate_password(flags, n);
		if (ret != 0)
			return ret;
	}

	return ret;
}
