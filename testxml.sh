#!/bin/sh

gcc -I /usr/include/libxml2 -I /usr/include/freetype2 -Wall -g -c -o libxscroller-static.o libxscroller.c
echo "Stage 1 done"
# Create static library.
ar rcs libxscroller-static.a libxscroller-static.o
echo "Stage 2 done"
# Compile test program file.
gcc -I /usr/include/libxml2 -I /usr/include/freetype2 -Wall -g -c testxml.c -o testxml.o
echo "Stage 3 done"
gcc -g -o testxml testxml.o  -I /usr/X11R6/include -I /usr/include/freetype2 -I /usr/include/X11/Xft -I /usr/include/libxml2 -L. -L/usr/X11/lib -L/usr/lib -lxscroller-static -lX11 -lXext -lXft -lpng -ljpeg -ltiff -lgif -lz -lm -lImlib -lxml2 -lpthread
echo "Stage 4 done"
# Execute the program.
./testxml
