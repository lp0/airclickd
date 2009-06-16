airclickd: airclickd.c Makefile
	gcc -o airclickd airclickd.c -O2 -Wall -Werror -ggdb

clean:
	rm -f airclickd
