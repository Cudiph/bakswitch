SRCS = main.c vecstr.c
OBJS = ${SRCS:.c=.o}

bakswitch: ${OBJS}
	${CC} ${OBJS} -O3 -o bakswitch

main.o: vecstr.h

install: bakswitch
	mv ./bakswitch /bin/

uninstall:
	rm /bin/bakswitch

clean:
	rm -f bakswitch ${OBJS}

.PHONY: clean install uninstall
