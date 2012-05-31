LibreLibreCell is a free FreeCell clone written in C++, using QT for 
the graphical interface.

Since it is QT and C++, LibreLibreCell should work in Linux, Windows 
and OS X. However, I have only tested it in Arch Linux.

To build LibreLibreCell, run the following commands:
$ mkdir build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
# make install

Replace "/usr" with whatever location you want.
The files will be installed in
CMAKE_INSTALL_PREFIX/bin/
and
CMAKE_INSTALL_PREFIX/share/doc/librelibrecell/


LibreLibreCell does not copy FreeCell exactly. The rules are the same, 
of course, but I have done some other things differently:
- No popups at all. Messages go in the text box at the bottom of the 
window.
- Moving a single card to an empty column is done with ctrl-click.
- The algorithm that decides when cards jump to the right corner on 
their own may be slightly different (not on purpose).
- The keyboard shortcuts for the menu.
- Double click on a card always moves it to the left corner (if there 
is a free space).
- It is possible to use any card sets, as long as the files are named 
the way LibreLibreCell expects them (see below).
- Selecting and immediately deselecting a card doesn't disable undo.

Some features of FreeCell aren't implemented yet:
- The keyboard equivalent of right clicking on a card.
- Statistics.

Some features of FreeCell I don't plan to implement:
- Animations - cards disappear from their source and appear at their
destination.
- Changing the mouse cursor.
- Green background.
- Flashing taskbar entry.


The cards are supposed to be called "<type>-<number>.png", where <type> 
is one of "spades", "hearts", "clubs" or "diamonds", and number is 
between 00 and 12. Number 00 is the ace, number 12 is the king.


The two card sets distributed with LibreLibreCell, "Swiss Costumes" and 
"Paris Pattern", are supposed to be in the public domain, according to 
http://www.rahga.com/ggsvg/. They are obviously not my own work - I 
just grabbed them from that website.


Home:
https://github.com/dubhater/LibreLibreCell
