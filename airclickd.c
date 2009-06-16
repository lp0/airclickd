/*
	2009-06-16 Simon Arlott
	http://simon.arlott.org/sw/airclickd/

	The code is released into the public domain.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define AC_PLAY     0x01
#define AC_VOLUP    0x02
#define AC_VOLDOWN  0x04
#define AC_NEXT     0x08
#define AC_PREV     0x10

/*
	Bytes 0, 1, 2:
	03 02 00

	Byte 3:
	01 play
	02 up
	04 down
	08 next
	10 prev
	20 n/a
	40 n/a
	80 n/a

	Byte 4:
	01 !play
	02 !up
	04 down
	08 (!next) ^ down
	10 (!prev) ^ (next && down)
	20 !(prev && next && down)
	40 n/a
	80 n/a

	Bytes 3, 4, 5:
	00 00 00
*/

#define AC_LEN      8
#define AC_STATUS   3

#define AC_SIG1_POS 0
#define AC_SIG1_VAL 0x03
#define AC_SIG2_POS 1
#define AC_SIG2_VAL 0x02
#define AC_SIG3_POS 2
#define AC_SIG3_VAL 0x00
#define AC_SIG4_POS 5
#define AC_SIG4_VAL 0x00
#define AC_SIG5_POS 6
#define AC_SIG5_VAL 0x00
#define AC_SIG6_POS 7
#define AC_SIG6_VAL 0x00

int run(const char *cmd);

int main() {
	unsigned char last = 0;
	unsigned char buf[AC_LEN];
	int status;
	int children = 0;

	/* We'll read 8 bytes (repeatedly) whenever a button is pressed, held down or released. */
	while (read(0, &buf, sizeof(buf)) == sizeof(buf)) {
		if (children > 0) {
			if (waitpid(-1, &status, WNOHANG) > 0)
				children--;
		}

		if (buf[AC_SIG1_POS] != AC_SIG1_VAL) continue;
		if (buf[AC_SIG2_POS] != AC_SIG2_VAL) continue;
		if (buf[AC_SIG3_POS] != AC_SIG3_VAL) continue;
		if (buf[AC_SIG4_POS] != AC_SIG4_VAL) continue;
		if (buf[AC_SIG5_POS] != AC_SIG5_VAL) continue;
		if (buf[AC_SIG6_POS] != AC_SIG6_VAL) continue;

		if (!(last & AC_PLAY) && (buf[AC_STATUS] & AC_PLAY))
			children += run("airclick-play");

		if (!(last & AC_VOLUP) && (buf[AC_STATUS] & AC_VOLUP))
			children += run("airclick-volup");

		if (!(last & AC_VOLDOWN) && (buf[AC_STATUS] & AC_VOLDOWN))
			children += run("airclick-voldown");

		if (!(last & AC_NEXT) && (buf[AC_STATUS] & AC_NEXT))
			children += run("airclick-next");

		if (!(last & AC_PREV) && (buf[AC_STATUS] & AC_PREV))
			children += run("airclick-prev");

		last = buf[AC_STATUS];
	}
	perror("read");
	return 1;
}

int run(const char *cmd) {
	if (fork() != 0) return 1;

	close(0);
	close(1);
	close(2);

	execlp(cmd, cmd, NULL);
	_exit(1);
}
