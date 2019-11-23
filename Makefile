CC=gcc
CFLAGS=-shared -fPIC
CFLAGS+=-I/usr/include/mpv/
LDFLAGS=-lsystemd

plugin.so: plugin.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

scripts:
	install -d ~/.config/mpv/scripts

install: plugin.so scripts
	install plugin.so ~/.config/mpv/scripts

develop: plugin.so scripts
	ln plugin.so ~/.config/mpv/scripts/night-light.so
