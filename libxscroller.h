/* Stop multiple inclusions */
#ifndef _HAVE_LIBXSROLLER_
#define _HAVE_LIBXSCROLLER_

/* libxscroller.h - header files for libxscroller. */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#define DEBUG

/* Type definitions */
typedef struct {													/* structure to hold the xml info and connection */
	char _xmlFile[50];												/* Filename to parse */
	xmlDocPtr doc;													/* Pointer to the document */
	xmlNodePtr cur;													/* Cursor position in document */
} xmlInfo;
typedef struct {													/* Structure of display info */
	Display *dpy;													/* Connection to display */
	xmlNodePtr xmlCur;												/* The cursor point for xml doc */
	char _name[50];													/* Name of server/display */
	int _screen;													/* Screen to use on display */
} dpyInfo;
typedef struct {													/* Structure holding windows information */
	Window win;														/* Connection to the window */
	xmlNodePtr xmlCur;												/* The cursor point for xml doc */
	char _name[50];													/* Name of the window */
	char _id[20];													/* Id of the window */
	char _bg[5];													/* The background colour */
	int _border;													/* Border width of the window */
	char _bc[5];													/* Border colour */
	int _x;															/* X Position of the window */
	int _y;															/* Y Position of the window */
	unsigned int _w;												/* Width of the window */
	unsigned int _h;												/* Height of the window */
} winInfo;
typedef struct {													/* Structure holding buffer information */
	Pixmap buffer;													/* Pixmap for our buffer */
	GC gc,clear;													/* Graphic context */
	unsigned long valuemask;										/* Value mask */
	XGCValues values,clearvalues;									/* GC Values */
	int _x;															/* The X offset for the buffer */
	int _wTotal;													/* Total combined X */
	int _hTotal;													/* Height of the highest object */
	int _y;															/* The Y offset for the buffer */
} bufInfo;
typedef struct {													/* Image struct */
	ImlibData *iData;												/* The image data */
	ImlibImage *iFull;												/* The image */
	Pixmap p,m;														/* Pixmaps */
	char _name[50];													/* The file name */
	int _x;															/* Image X */
	int _y;															/* Image Y */
	int _iw;														/* Initial width */
	int _ih;														/* Initial height */
	int _w;															/* Image W */
	int _h;															/* Image H */
	int _space;														/* Space in between img & text */
} imgInfo;
typedef struct {													/* Structure for text */
	XftFont *font;													/* The connection to the font */
	XftDraw *draw;													/* The connection to the XftDraw */
	XftColor fgColour;												/* The output colour */
	char _text[100];												/* The text to be displayed */
	char _font[50];													/* The text font */
	int _size;														/* Font size */
	int _space;														/* Padding in beginning of text */
	int _x;															/* The texts X offset */
	int _y;															/* The texts Y offset */
	int _w;															/* The text width */
	int _h;															/* The text height */
	int _r;															/* The text RED value */
	int _g;															/* The text GREEN value */
	int _b;															/* The text BLUE value */
	int _a;															/* The text ALPHA value */
} txtInfo;
typedef struct {													/* Structure for easing properties */
	int _d;															/* Easing duration */
	int _t;															/* Easing time */
	int _diff;														/* The ease difference */
	int _init;														/* The initial value */
	int _cycles;													/* How many loops to do */
	char _type[10];													/* The ease type */
	char _direction[10];											/* Direction to ease */
	char _align[10];												/* Text alignment */
} eseInfo;
typedef struct {
	int _total;														/* Total quantity of objects(text/images) */
	xmlNodePtr xmlCur[10];											/* Pointer to the xml location */
	char _type[10][6];												/* The object type (image/text) */
} objInfo;
/* Structure definitions */
dpyInfo display;	/* FIXME: This needs to be created from the xml function loop */
struct ptrInfo pointer;

struct ptrInfo {													/* Structure for the pointer */
	Window win;														/* Create a 1 pixel window */
	Pixmap pix;														/* The cursor pixmap */
	XColor colour;													/* The colour for the cursor */
	Cursor blank;													/* The blank cursor */
	int _x;															/* The X location */
	int _y;															/* The Y Location */
};
/* FIXME: This needs to be changed to a typedef */
struct dlyInfo delay;
struct dlyInfo {													/* Structure for delay */
	struct timeval st, rt;											/* The time avaluations */
	int _d;															/* Difference in time */
	int _s;															/* Speed int */
};
/* Xml Functions */
xmlInfo loadXml(char xmlFile[50]);									/* Parse xml */
dpyInfo parseDisplay(xmlInfo *xmlDocPtr);							/* Grab displays out of xml */
winInfo parseWindow(xmlDocPtr doc);									/* Grab windows out of xml */
void parseObjects(													/* Parse and execute objects */
	xmlDocPtr doc,
	winInfo *w,
	xmlNodePtr cur);
void parseGeometry(													/* Grab window geometry */
	xmlDocPtr doc,
	winInfo *windowXml,
	xmlNodePtr cur);
void parseBorder(													/* Grab window border information */
	xmlDocPtr doc,
	winInfo *windowXml,
	xmlNodePtr cur);
objInfo parseContent(												/* Grab window content */
	xmlDocPtr doc,
	winInfo *windowXml,
	bufInfo *b,
	xmlNodePtr cur);
void parseEase(														/* Parse easing and execute */
	xmlDocPtr doc,
	winInfo *windowXml,
	bufInfo *bufXml,
	xmlNodePtr cur);
imgInfo parseImage(													/* Parse image and create */
	xmlDocPtr doc,
	winInfo *windowXml,
	bufInfo *bufXml,
	xmlNodePtr cur);
txtInfo parseText(													/* Parse text and create */
	xmlDocPtr doc,
	winInfo *windowXml,
	bufInfo *bufXml,
	xmlNodePtr cur);
/* Display functions */
void createDisplay();												/* Create a connection to the display */
void destroyDisplay();												/* Close connection to the display */
/* Window functions */
void createWindow(winInfo *w);										/* Create a window */
void destroyWindow(winInfo *w);										/* Destroy a window */
void setWindow(winInfo *w);											/* Set window geometry */
void nameWindow(winInfo *w);										/* Name the window */
void topWindow(winInfo *w);											/* Place window on top of all others */
void hideWindow(winInfo *w);										/* Hide the window */
/* Pointer functions */
void setPointer();													/* Set pointers position */
void hidePointer();													/* Hide the pointer */
/* Ease functions */
int easeIn(eseInfo *e);												/* Ease value in */
int easeOut(eseInfo *e);											/* Ease value out */
int easeIo(eseInfo *e);												/* Ease value in half way, then out */
int easeOi(eseInfo *e);												/* Ease value out half way, the in */
int easeNone(eseInfo *e);											/* Linear inc/dec of value */
/* Buffer functions */
void createBuffer(winInfo *w, bufInfo *b);							/* Create a pixmap buffer */
void clearBuffer(winInfo *w, bufInfo *b);							/* Clear contents of pixmap(draw a rect) */
void showBuffer(winInfo *w, bufInfo *b);							/* Place buffer in the window */
void destroyBuffer(bufInfo *b);										/* Destroy the pixmap */
/* Text functions */
void createText(bufInfo *b, txtInfo *t);							/* Create some text */
void addText(winInfo *w,bufInfo *b, txtInfo *t);					/* Place text in the buffer */
/* Image functions */
void readImage(bufInfo *b,imgInfo *i);								/* Read in an image file */
void addImage(winInfo *w, bufInfo *b, imgInfo *i);					/* Add the image to the buffer */
void destroyImage(imgInfo *i);										/* Clear image from memory */
/* Delay functions */
int timeDelay(int i);												/* Create a delay (pause) */
int timeDiff();														/* Calculate function execution time */

#endif
