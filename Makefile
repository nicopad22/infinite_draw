CC=clang
CFILE=src/gtkdraw.c
OUTFILE=gtkdraw

gtkdraw:
	$(CC) `pkg-config --cflags gtk+-3.0` -o $(OUTFILE) $(CFILE) `pkg-config --libs gtk+-3.0`

clean:
	rm $(OUTFILE)