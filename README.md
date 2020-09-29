# Welcome to gtkdraw

This is an infinite canvas app written in `c` using gtk for illustrating ideas and planning projects.

## TODO

I've just started, so this would be the TODO list:

- Create basic code structure (vectors, lines, basic utility functions)
- Get basic drawing window set up
- Save as gtkdraw project (.gdw)
- Save as image feature (.bmp first, maybe .jpg or .png later)

## How it works

This app works in, well, the way that seemed most reasonable to me. We basically use a linked list to store each frame when something interesting happened (we clicked, we dragged the cursor, etc) And each of those nodes has a state. This state helps when drawing the lines onto the surface, helpswhen working with the lists as in order to facilitate drawing, we have to append to the end. instead of iterating the whole linked list, this program stores the last node, and uses that to append the new nodes, and update the already existing node's states.
