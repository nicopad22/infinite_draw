CC=clang
CFILE=src/gtkdraw.c
OUTFILE=gtkdraw

gtkdraw: src/gtkdraw.c
	$(CC) `pkg-config --cflags gtk+-3.0` -o $(OUTFILE) $(CFILE) `pkg-config --libs gtk+-3.0`

clean:
	rm $(OUTFILE)