#!/bin/sh

gcc -I /usr/include/libxml2 -I /usr/include/freetype2 -Wall -g -c -o libxscroller-static.o libxscroller.c
echo "3 Stages left"
# Create static library.
ar rcs libxscroller-static.a libxscroller-static.o
echo "2 Stages left"
# Compile test program file.
gcc -I /usr/include/libxml2 -I /usr/include/freetype2 -Wall -g -c xscroller.c -o xscroller.o
echo "1 Stage left"
gcc -g -o xscroller xscroller.o  -I /usr/X11R6/include -I /usr/include/freetype2 -I /usr/include/X11/Xft -I /usr/include/libxml2 -L. -L/usr/X11/lib -L/usr/lib -lxscroller-static -lX11 -lXext -lXft -lpng -ljpeg -ltiff -lgif -lz -lm -lImlib -lxml2 -lpthread
echo "Completed compile"
# Execute the program.
./xscroller

