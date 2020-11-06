# Welcome to infinite draw (by nicopad22)

This is an infinite canvas app written in `c` using gtk for illustrating ideas and planning projects, it allows you to draw in an infinite canvas where you can zoom in and zoom out a lot, and not lose any quality in the process, as this is *not* a pixel-based app like paint.

Yes, I know milton exists, but this was my attemt at making a similar thing, with my own way of making it, and my own features, etc. (plus, it should work on every OS that gtk3 supports, hasnt been tested though)

Feedback is really appreciated, you can contact me through discord (Кинтана#3752).

## How to compile and run

To compile the app, simply open a terminal, go to the directory in which the `Makefile` resides, and run the `make` command. This will generate an `infinite_draw` binary file in the directory, which you can then run with `./infinite_draw`. This is a bit of an archaic way, and I do plan on making an .exe and mac and linux stuff, but didnt have the time right now, so we stick to this.

## How to use

To use this app, there's simple controls:

- To draw on the screen, simply drag across it, it will draw a line with the stroke width and color selected.

- On the left, there's a slider for selecting the brush size and a button which if clicked will open up a menu to select the color.

- To move across the canvas, press space to start moving, and press space again to stop.

## TODO

- [x] Create basic code structure (vectors, lines, basic utility functions)
- [x] Get basic drawing window set up
- [x] Zoom towards mouse
- [x] Color drawing (+ selecting size)
- [ ] Save as infinite draw project (.ind)
- [ ] Save as image feature (.bmp first, maybe .jpg or .png later)
- [ ] Bookmarks
