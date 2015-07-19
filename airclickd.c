/*
	2009-06-16 Simon Arlott
	http://simon.arlott.org/sw/airclickd/

	The code is released into the public domain.
*/

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define AC_PLAY     0x01
#define AC_VOLUP    0x02
#define AC_VOLDOWN  0x04
#define AC_NEXT     0x08
#define AC_PREV     0x10

/*
	Protocol format:
	03 02 00 xx xx 00 00 00
	        /     \
	play  01       01 !play
	up    02       02 !up
	down  04       04 down
	next  08       08 (!next) ^ down
	prev  10       10 (!prev) ^ (next && down)
	n/a   20       20 !(prev && next && down)
	n/a   40       40 n/a
	n/a   80       80 n/a
*/

#define AC_LEN      8
#define AC_STATUS   3

#define AC_PREFIX   0
#define AC_SUFFIX   5

static const uint8_t ac_prefix[] = { 0x03, 0x02, 0x00 };
static const uint8_t ac_suffix[] = { 0x00, 0x00, 0x00 };

static void run(const char *cmd);

int main() {
	uint_fast8_t last = 0;
	uint8_t buf[AC_LEN];

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		perror("signal");
		return EXIT_FAILURE;
	}

	/* We'll read 8 bytes (repeatedly) whenever a button is pressed, held down or released. */
	while (read(STDIN_FILENO, &buf, sizeof(buf)) == sizeof(buf)) {
		uint_fast8_t current = buf[AC_STATUS];

		if (current == last)
			continue;

		if (memcmp(buf+AC_PREFIX, ac_prefix, sizeof(ac_prefix)) || memcmp(buf+AC_SUFFIX, ac_suffix, sizeof(ac_suffix)))
			continue;

		if ((current & AC_PLAY) && !(last & AC_PLAY))
			run("airclick-play");

		if ((current & AC_VOLUP) && !(last & AC_VOLUP))
			run("airclick-volup");

		if ((current & AC_VOLDOWN) && !(last & AC_VOLDOWN))
			run("airclick-voldown");

		if ((current & AC_NEXT) && !(last & AC_NEXT))
			run("airclick-next");

		if ((current & AC_PREV) && !(last & AC_PREV))
			run("airclick-prev");

		last = current;
	}
	perror("read");
	return EXIT_FAILURE;
}

static void run(const char *cmd) {
	if (fork() != 0) return;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	execlp(cmd, cmd, NULL);
	_exit(1);
}
