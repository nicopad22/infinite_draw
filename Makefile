CC=clang
CFILE=src/infinite_draw.c
HFILE=src/infinite_draw.h
OUTFILE=infinite_draw

gtkdraw: $(CFILE) $(HFILE)
	$(CC) `pkg-config --cflags gtk+-3.0` -o $(OUTFILE) $(CFILE) `pkg-config --libs gtk+-3.0` -lm

clean:
	rm $(OUTFILE)