CC=clang
CFILES=src/infinite_draw.c
HFILES=src/infinite_draw.h
OUTFILE=infinite_draw

gtkdraw: $(CFILES) $(HFILES)
	$(CC) `pkg-config --cflags gtk+-3.0` -o $(OUTFILE) $(CFILES) `pkg-config --libs gtk+-3.0` -lm

clean:
	rm $(OUTFILE)