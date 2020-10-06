CC=clang
CFILE=src/gtkdraw.c
OUTFILE=gtkdraw

gtkdraw: src/gtkdraw.c src/gtkdraw.h
	$(CC) `pkg-config --cflags gtk+-3.0` -o $(OUTFILE) $(CFILE) `pkg-config --libs gtk+-3.0` -lm

clean:
	rm $(OUTFILE)