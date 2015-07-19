airclickd: airclickd.c Makefile
	gcc -D_POSIX_C_SOURCE=200112L -D_ISOC99_SOURCE -D_GNU_SOURCE -O2 -Wall -Wextra -Wshadow -Werror -ggdb -pipe -o airclickd airclickd.c

clean:
	rm -f airclickd
