#!/bin/sh

gcc -Wall -g -c -o libppx-static.o libppx.c

# Create static library.
ar rcs libppx-static.a libppx-static.o

# Compile test program file.
gcc -Wall -g -c testxml.c -o testxml.o

gcc -g -o testxml testxml.o  -I/usr/X11R6/include -I/usr/include/freetype2 -I/usr/include/X11/Xft -I/usr/include/libxml2 -L. -L/usr/X11/lib -L/usr/lib -lppx-static -lX11 -lXext -lXft -lpng -ljpeg -ltiff -lgif -lz -lm -lImlib -lxml2 -lpthread

# Execute the program.
./testxml
