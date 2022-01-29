SHELL = /bin/sh

OBJS = overlay.o
CFLAGS = -Wall -g -I /usr/include/cairo
LDFLAGS = -lX11 -lXfixes -lXcomposite -lcairo
CC = gcc
EXECUTABLE = overlay

ifeq (${PREFIX},)
    PREFIX := /usr/local
endif

${EXECUTABLE}:${OBJS}
	${CC} ${OBJS} -o $@ ${CFLAG} $(LDFLAGS)

clean:
	rm -f ${OBJS}

install:
	install -Dm755 ${EXECUTABLE} ${DESTDIR}${PREFIX}/bin/
