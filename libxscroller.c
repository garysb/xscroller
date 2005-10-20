/* libxscroller.c - xscroller library */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include "libxscroller.h"

/************************************************************************/
/*							Xml functions								*/
/************************************************************************/
xmlInfo loadXml(char xmlFile[50]) {										/* Load an xml file into memory */
	xmlInfo tmpXml;														/* Create tmp information */
	strcpy(tmpXml._xmlFile,xmlFile);									/* Copy filename into structure */
	tmpXml.doc = xmlParseFile(tmpXml._xmlFile);							/* Create doc in structure */
	if (tmpXml.doc == NULL ) {											/* If no document */
		fprintf(stderr,"Document not parsed successfully.\n");			/* Print error */
		return tmpXml;													/* And return to main */
	}
	tmpXml.cur = xmlDocGetRootElement(tmpXml.doc);						/* Start cursor at begining of document */
	if (tmpXml.cur == NULL) {											/* If cursor is blank */
		fprintf(stderr,"empty document\n");								/* Print error */
		xmlFreeDoc(tmpXml.doc);											/* And free the document */
		return tmpXml;													/* Return to main */
	}
	if (xmlStrcmp(tmpXml.cur->name, (const xmlChar *) "ppx")) {			/* If the root isnt ppx */
		fprintf(stderr,													/* Print error */
			"document of the wrong type, root node != ppx\n");
		xmlFreeDoc(tmpXml.doc);											/* Clear document */
		return tmpXml;													/* Return to main */
	}
	tmpXml.cur = tmpXml.cur->xmlChildrenNode;							/* Shift cursor */
	return tmpXml;														/* Return to main */
}
dpyInfo parseDisplay(xmlInfo *xmlDocPtr) {
	dpyInfo displayTmp;
	xmlChar *key;
	xmlNodePtr cur1, cur2;
	cur1 = xmlDocPtr->cur;
	while (cur1 != NULL) {
		if ((!xmlStrcmp(cur1->name, (const xmlChar *)"display"))){
			cur2 = cur1->xmlChildrenNode;
			displayTmp.xmlCur = cur2;	
			while (cur2 != NULL) {
				if ((!xmlStrcmp(cur2->name, (const xmlChar *)"name"))) {
					key = xmlNodeListGetString(xmlDocPtr->doc, cur2->xmlChildrenNode, 1);
					strcpy(displayTmp._name,key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(cur2->name, (const xmlChar *)"screen"))) {
					key = xmlNodeListGetString(xmlDocPtr->doc, cur2->xmlChildrenNode, 1);
					displayTmp._screen = atoi(key);
					xmlFree(key);
				}
				cur2 = cur2->next;
			}
		}
		cur1 = cur1->next;
	}
	return displayTmp;
}
winInfo parseWindow(xmlDocPtr doc) {
	winInfo windowTmp;
	xmlNodePtr cur;
	xmlChar *key;
	cur = display.xmlCur->xmlChildrenNode;
	windowTmp.xmlCur = cur;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(windowTmp._name,key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"bgcolour"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(windowTmp._bg,key);
			xmlFree(key);
		}
	cur = cur->next;
	}
	parseGeometry(doc,&windowTmp,windowTmp.xmlCur);
	parseBorder(doc,&windowTmp,windowTmp.xmlCur);
	return windowTmp;
}
void parseObjects(xmlDocPtr doc,winInfo *w,xmlNodePtr cur) {
	xmlNodePtr curg;
	curg = cur->xmlChildrenNode;
	bufInfo b;
	b._x = 0;
	createBuffer(w,&b);
	clearBuffer(w,&b);
	XFlush(display.dpy);
	parseEase(doc,w,&b,curg);
	return;
}
void parseGeometry(xmlDocPtr doc, winInfo *windowXml,xmlNodePtr cur) {
	xmlNodePtr curg;
	xmlChar *key;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"geometry"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			curg = cur->xmlChildrenNode;
			while (curg != NULL) {
				if ((!xmlStrcmp(curg->name, (const xmlChar *)"width"))) {
					key = xmlNodeListGetString(doc, curg->xmlChildrenNode, 1);
					windowXml->_w = atoi(key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(curg->name, (const xmlChar *)"height"))) {
					key = xmlNodeListGetString(doc, curg->xmlChildrenNode, 1);
					windowXml->_h = atoi(key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(curg->name, (const xmlChar *)"xcoord"))) {
					key = xmlNodeListGetString(doc, curg->xmlChildrenNode, 1);
					windowXml->_x = atoi(key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(curg->name, (const xmlChar *)"ycoord"))) {
					key = xmlNodeListGetString(doc, curg->xmlChildrenNode, 1);
					windowXml->_y = atoi(key);
					xmlFree(key);
				}
			curg = curg->next;
			}
		}
		cur = cur->next;
	}
	return;
}
void parseBorder(xmlDocPtr doc, winInfo *windowXml,xmlNodePtr cur) {
	xmlNodePtr curb;
	xmlChar *key;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"border"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			curb = cur->xmlChildrenNode;
			while (curb != NULL) {
				if ((!xmlStrcmp(curb->name, (const xmlChar *)"width"))) {
					key = xmlNodeListGetString(doc, curb->xmlChildrenNode, 1);
					windowXml->_border = atoi(key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(curb->name, (const xmlChar *)"colour"))) {
					key = xmlNodeListGetString(doc, curb->xmlChildrenNode, 1);
					strcpy(windowXml->_bc,key);
					xmlFree(key);
				}
			curb = curb->next;
			}
		}
		cur = cur->next;
	}
	return;
}
objInfo parseContent(xmlDocPtr doc, winInfo *windowXml,bufInfo *bufXml,xmlNodePtr cur) {
	xmlNodePtr curc;
	xmlChar *key;
	objInfo objTmp;
	int cnt = 0;
	objTmp._total = 0;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"content"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			curc = cur->xmlChildrenNode;
			while (curc != NULL) {
				if ((!xmlStrcmp(curc->name, (const xmlChar *)"image"))) {
					key = xmlNodeListGetString(doc, curc->xmlChildrenNode, 1);
					cnt++;
					objTmp._total++;
					strcpy(objTmp._type[cnt],"image");
					objTmp.xmlCur[cnt] = curc->xmlChildrenNode;
					xmlFree(key);
				}
				if ((!xmlStrcmp(curc->name, (const xmlChar *)"write"))) {
					key = xmlNodeListGetString(doc, curc->xmlChildrenNode, 1);
					cnt++;
					objTmp._total++;
					strcpy(objTmp._type[cnt],"write");
					objTmp.xmlCur[cnt] = curc->xmlChildrenNode;
					xmlFree(key);
				}
				curc = curc->next;
				xmlCleanupParser();
			}
		}
		cur = cur->next;
		xmlCleanupParser();
	}
	return objTmp;
}
void parseEase(xmlDocPtr doc, winInfo *windowXml,bufInfo *bufXml,xmlNodePtr cur) {
	xmlNodePtr cure,curc;
	curc = cur;
	xmlChar *key;
	eseInfo easer[5];
	imgInfo images[5];
	txtInfo texts[5];
	int i = 0;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"ease"))) {
			i++;
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			cure = cur->xmlChildrenNode;
			while (cure != NULL) {
				if ((!xmlStrcmp(cure->name, (const xmlChar *)"type"))) {
					key = xmlNodeListGetString(doc, cure->xmlChildrenNode, 1);
					strcpy(easer[i]._type,key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(cure->name, (const xmlChar *)"direction"))) {
					key = xmlNodeListGetString(doc, cure->xmlChildrenNode, 1);
					strcpy(easer[i]._direction,key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(cure->name, (const xmlChar *)"align"))) {
					key = xmlNodeListGetString(doc, cure->xmlChildrenNode, 1);
					strcpy(easer[i]._align,key);
					xmlFree(key);
				}
				if ((!xmlStrcmp(cure->name, (const xmlChar *)"cycles"))) {
					key = xmlNodeListGetString(doc, cure->xmlChildrenNode, 1);
					easer[i]._cycles = atoi(key);
					objInfo objList;
					objList = parseContent(doc,windowXml,bufXml,curc);
					bufXml->_y = 0;
					bufXml->_x = 0;
					bufXml->_wTotal = 0;
					bufXml->_hTotal = 0;
					easer[i]._diff = 0;
					easer[i]._init = 0;
					int xincr = 1;
					while (xincr <= objList._total){
						/* FIXME: createing things for all is NOT good */
						if (!strcmp(objList._type[xincr],"image")){
							images[xincr] = parseImage(doc,windowXml,bufXml,objList.xmlCur[xincr]);
						}
						if (!strcmp(objList._type[xincr],"write")){
							texts[xincr] = parseText(doc,windowXml,bufXml,objList.xmlCur[xincr]);
						}
						xincr++;
					}
					xincr = 0;
					while (xincr <= objList._total){
						/* FIXME: createing things for all is NOT good */
						if (!strcmp(objList._type[xincr],"image")){
							addImage(windowXml,bufXml,&images[xincr]);
						}
						if (!strcmp(objList._type[xincr],"write")){
							addText(windowXml,bufXml,&texts[xincr]);
						}
						xincr++;
					}
					xmlFree(key);
					delay._s = 8000;
					if (!strcmp(easer[i]._direction,"left")){
						easer[i]._diff = 0-windowXml->_w-bufXml->_wTotal;
						easer[i]._init = windowXml->_w;
						easer[i]._d = windowXml->_w+bufXml->_wTotal;
					}
					if (!strcmp(easer[i]._direction,"right")){
						easer[i]._diff = windowXml->_w+bufXml->_wTotal;
						easer[i]._init = 0-bufXml->_wTotal;
						easer[i]._d = windowXml->_w+bufXml->_wTotal;
					}
					if (!strcmp(easer[i]._direction,"up")){
						easer[i]._diff = 0-(windowXml->_h+(bufXml->_hTotal*2));
						easer[i]._init = windowXml->_h;
						easer[i]._d = easer[i]._diff*(-1);
					}
					if (!strcmp(easer[i]._direction,"down")){
						easer[i]._diff = windowXml->_h+(bufXml->_hTotal);
						easer[i]._init = 0-bufXml->_hTotal;
						easer[i]._d = easer[i]._diff;
					}
					while(easer[i]._cycles >= 1) {
						gettimeofday(&delay.st, NULL);
						easer[i]._t = 0;
						int pause = 0;
						while (easer[i]._t <= easer[i]._d) {
							clearBuffer(windowXml,bufXml);
							if (!strcmp(easer[i]._direction,"left") || !strcmp(easer[i]._direction,"right")){
								if (!strcmp(easer[i]._align,"top")){
									bufXml->_y = 0;
								}
								if (!strcmp(easer[i]._align,"bottom")){
									bufXml->_y = (windowXml->_h-bufXml->_hTotal);
								}
								if (!strcmp(easer[i]._align,"center")){
									bufXml->_y = windowXml->_h/2-20;
								}
								if (!strcmp(easer[i]._type,"none")){
									bufXml->_x = easeNone(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"in")){
									bufXml->_x = easeIn(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"out")){
									bufXml->_x = easeOut(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"io")){
									bufXml->_x = easeIo(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"oi")){
									bufXml->_x = easeOi(&easer[i]);	
								}
							}
							if (!strcmp(easer[i]._direction,"up") || !strcmp(easer[i]._direction,"down")){
								if (!strcmp(easer[i]._align,"left")){
									bufXml->_x = 0;
								}
								if (!strcmp(easer[i]._align,"right")){
									bufXml->_x = windowXml->_w-bufXml->_wTotal;
								}
								if (!strcmp(easer[i]._align,"center")){
									bufXml->_x = (windowXml->_w/2)-(bufXml->_wTotal/2);
								}
								bufXml->_wTotal = 0;
								if (!strcmp(easer[i]._type,"none")){
									bufXml->_y = easeNone(&easer[i]);
								}
								if (!strcmp(easer[i]._type,"in")){
									bufXml->_y = easeIn(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"out")){
									bufXml->_y = easeOut(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"io")){
									bufXml->_y = easeIo(&easer[i]);	
								}
								if (!strcmp(easer[i]._type,"oi")){
									bufXml->_y = easeOi(&easer[i]);	
								}
							}
							xincr = 0;
							while (xincr <= objList._total){
								if (!strcmp(objList._type[xincr],"image")){
									addImage(windowXml,bufXml,&images[xincr]);
								}
								if (!strcmp(objList._type[xincr],"write")){
									addText(windowXml,bufXml,&texts[xincr]);
								}
								xincr++;
							}
							showBuffer(windowXml,bufXml);
							XFlush(display.dpy);
							XSync(display.dpy,True);
							gettimeofday(&delay.rt, NULL);
							pause = timeDelay(delay._s - timeDiff() + pause);
							delay.st = delay.rt;
							easer[i]._t++;
						}
						easer[i]._cycles--;
					}
				}
				cure = cure->next;
			}
		}
		cur = cur->next;
	}
	return;
}
imgInfo parseImage(xmlDocPtr doc, winInfo *windowXml,bufInfo *bufXml,xmlNodePtr cur) {
	xmlChar *key;
	imgInfo cImage;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(cImage._name,key);
			xmlFree(key);
			}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"space"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
 			cImage._space = atoi(key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"width"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
 			cImage._w = atoi(key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"height"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
 			cImage._h = atoi(key);
			xmlFree(key);
			readImage(bufXml,&cImage);
		}
		cur = cur->next;
	}
	return cImage;
}
txtInfo parseText(xmlDocPtr doc, winInfo *windowXml,bufInfo *bufXml,xmlNodePtr cur) {
	xmlChar *key;
	txtInfo cText;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"space"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			cText._space = atoi(key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"font"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(cText._font,key);
			xmlFree(key);
			}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"colour"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (!strcmp(key, "black")) {
				cText._r = 0;									/* The text RED value */
				cText._g = 0;									/* The text GREEN value */
				cText._b = 0;									/* The text BLUE value */
				cText._a = 65025;								/* The text ALPHA value */
			}
			if (!strcmp(key, "white")) {
				cText._r = 65025;								/* The text RED value */
				cText._g = 65025;								/* The text GREEN value */
				cText._b = 65025;								/* The text BLUE value */
				cText._a = 65025;								/* The text ALPHA value */
			}
			if (!strcmp(key, "red")) {
				cText._r = 65025;								/* The text RED value */
				cText._g = 0;									/* The text GREEN value */
				cText._b = 0;									/* The text BLUE value */
				cText._a = 65025;								/* The text ALPHA value */
			}
			if (!strcmp(key, "green")) {
				cText._r = 0;									/* The text RED value */
				cText._g = 65025;								/* The text GREEN value */
				cText._b = 0;									/* The text BLUE value */
				cText._a = 65025;								/* The text ALPHA value */
			}
			if (!strcmp(key, "blue")) {
				cText._r = 0;									/* The text RED value */
				cText._g = 0;									/* The text GREEN value */
				cText._b = 65025;								/* The text BLUE value */
				cText._a = 65025;								/* The text ALPHA value */
			}
			xmlFree(key);
			}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"height"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			cText._size = atoi(key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"string"))) {
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(cText._text,key);
			xmlFree(key);
		createText(bufXml,&cText);
		}
		cur = cur->next;
	}
	return cText;
}
/************************************************************************/
/*						Display functions								*/
/************************************************************************/
void createDisplay()													/* Setup the display connection */
{
	if ((display.dpy = XOpenDisplay(display._name)) == NULL) {			/* If open display doesnt work */
#ifdef DEBUG															/* If DEBUG is turned on */
		printf ("Unable to open display '%s'\n",						/* Print error message */
		XDisplayName (display._name));									/* Print the name */
#endif																	/* End DEBUG */
		return;															/* Return ERROR */
	}
	display._screen = DefaultScreen(display.dpy);						/* Set the default screen(0) */
	XFlush(display.dpy);												/* Flush the connection to the server */
	return;																/* Return SUCCESS */
}
void destroyDisplay()													/* Destroy connection to display */
{
	XCloseDisplay(display.dpy);											/* Close the connection to dpy */
	return;																/* Return SUCCESS */
}
/************************************************************************/
/*						Window functions								*/
/************************************************************************/
void createWindow(winInfo *w)											/* Create a window */
{
	XSetWindowAttributes winattr;										/* Define win attribs */
	Visual *vis;														/* Create vis struct */
	winattr.override_redirect = True;									/* Set override redirect on */
	if (!strcmp(w->_bg, "white")) {										/* If background is set to white */
		winattr.background_pixel = WhitePixel(							/* Set the background */
			display.dpy,
			display._screen);
	}
	if (!strcmp(w->_bg, "black")) {										/* If background is set to black */
		winattr.background_pixel = BlackPixel(							/* Set the background */
			display.dpy,
			display._screen);
	}
	if (!strcmp(w->_bc, "white")) {										/* If background is set to white */
		winattr.border_pixel = WhitePixel(								/* Set the background */
			display.dpy,
			display._screen);
	}
	if (!strcmp(w->_bc, "black")) {										/* If background is set to black */
		winattr.border_pixel = BlackPixel(								/* Set the background */
			display.dpy,
			display._screen);
	}
	vis = DefaultVisual(display.dpy, display._screen);					/* Define vis as default screen visual */
	w->win = XCreateWindow(display.dpy,									/* Create a window */
						XRootWindow(display.dpy, display._screen),
						w->_x, w->_y,
						w->_w, w->_h,
						w->_border,
						CopyFromParent,
						InputOutput,
						vis,
						CWOverrideRedirect|CWBackPixel|CWBorderPixel,
						&winattr);
	XMapWindow(display.dpy,w->win);										/* Map the window */
	return;																/* Return success */
}
void destroyWindow(winInfo *w)											/* Destroy a window */
{
	XDestroyWindow(display.dpy, w->win);								/* Call destroy window */
	return;																/* Return success */
}
void setWindow(winInfo *w)												/* Set the windows x/y/w/h/b */
{
	XMoveResizeWindow(display.dpy, w->win,								/* Resize and move window */
			w->_x,
			w->_y,
			w->_w,
			w->_h);
	XSetWindowBorderWidth(display.dpy, w->win,							/* Set the border width */
			w->_border);
	return;																/* Return to main */
}
void nameWindow(winInfo *w)												/* Name the window */
{
	XStoreName(display.dpy,w->win, w->_name);								/* Send name to X */
	return;																/* Return to main */
}
void hideWindow(winInfo *w)												/* Hide the window */
{
	XUnmapWindow(display.dpy,w->win);									/* Unmap window */
	return;																/* Return to main */
}
void topWindow(winInfo *w)												/* Set window on top */
{
	XMapWindow(display.dpy,w->win);										/* Make sure window is mapped */
	XRaiseWindow(display.dpy,w->win);									/* Raise the window to the top */
	return;																/* Return to main function */
}
/************************************************************************/
/*						Pointer functions								*/
/************************************************************************/
void setPointer()														/* Set the pointers location */
{
	XWarpPointer(														/* Set pointer position */
		display.dpy,
		DefaultRootWindow(display.dpy),
		DefaultRootWindow(display.dpy),
		0,
		0,
		0,
		0,
		pointer._x,
		pointer._y);
	return;																/* Return to main */
}
void hidePointer()														/* Hide the pointer */
{
	pointer.pix = XCreatePixmap(
					display.dpy,
					DefaultRootWindow(display.dpy),
					1,
					1,
					1);
	pointer.colour.pixel = 0;
	pointer.colour.red = 0;
	pointer.colour.green = 0;
	pointer.colour.blue = 0;
	pointer.colour.flags = 0;
	pointer.colour.pad = 0;
	pointer.blank = XCreatePixmapCursor(
						display.dpy,
						pointer.pix,
						pointer.pix,
						&pointer.colour,
						&pointer.colour,
						1,
						1);
	XDefineCursor(display.dpy,
		DefaultRootWindow(display.dpy),
		pointer.blank );
	return;																/* Return to main */
}
/************************************************************************/
/*							Ease functions								*/
/************************************************************************/
int easeIn(eseInfo *e)													/* Ease in Function */
{
	float tmpTime = e->_t;												/* Create local time float */
	tmpTime /= e->_d;
	return e->_diff*tmpTime*tmpTime*tmpTime + e->_init;
}
int easeOut(eseInfo *e)													/* Ease out Function */
{
	float tmpTime = e->_t;
	tmpTime /= e->_d;
	tmpTime--;
	return e->_diff*(tmpTime*tmpTime*tmpTime + 1) + e->_init;
}
int easeIo(eseInfo *e)													/* Ease in/out Function */
{
	float tmpTime = e->_t;
	tmpTime /= e->_d/2;
	if (tmpTime < 1) {
		return e->_diff/2*tmpTime*tmpTime*tmpTime + e->_init;
	}
	tmpTime -= 2;
	return e->_diff/2*(tmpTime*tmpTime*tmpTime + 2) + e->_init;
}
int easeOi(eseInfo *e)													/* Ease in/out Function */
{
	float tmpTime = e->_t;
	float ts=(tmpTime/=e->_d)*tmpTime;
	float tc=ts*tmpTime;
	return e->_init+e->_diff*(4*tc + -6*ts + 3*tmpTime);
}
int easeNone(eseInfo *e)												/* No Easing, just scroll */
{
	float tmpTime = e->_t;
	return e->_diff*tmpTime/e->_d + e->_init;
}
/************************************************************************/
/*						Buffer functions								*/
/************************************************************************/
void createBuffer(winInfo *w, bufInfo *b)
{
	b->buffer = XCreatePixmap(											/* Create a pixmap buffer */
		display.dpy,
		w->win,
		w->_w,
		w->_h,
		DefaultDepth(display.dpy, 0));
	if (!strcmp(w->_bg, "black")) {									/* If background is set to black */
		b->values.background = BlackPixel(							/* Set the background */
			display.dpy,
			display._screen);
		b->values.foreground = b->values.background;
	}
	if (!strcmp(w->_bg, "white")) {									/* If background is set to black */
		b->values.background = WhitePixel(							/* Set the background */
			display.dpy,
			display._screen);
		b->values.foreground = b->values.background;
	}
	b->gc = XCreateGC(												/* Create graphic context for buffer */
		display.dpy,
		w->win,
		GCForeground | GCBackground,
		&b->values);
	if (!strcmp(w->_bg, "black")) {									/* If background is set to black */
		b->clearvalues.background = BlackPixel(						/* Set the background */
			display.dpy,
			display._screen);
		b->clearvalues.foreground = b->clearvalues.background;
	}
	if (!strcmp(w->_bg, "white")) {									/* If background is set to black */
		b->clearvalues.background = WhitePixel(						/* Set the background */
			display.dpy,
			display._screen);
		b->clearvalues.foreground = b->clearvalues.background;
	}
	b->clear = XCreateGC(
		display.dpy,
		w->win,
		GCForeground | GCBackground,
		&b->clearvalues);
	b->_wTotal = 0;
	return;
}
void clearBuffer(winInfo *w, bufInfo *b)
{
	b->_x = 0;
	b->_y = 0;
	XFillRectangle(display.dpy,
		b->buffer,
		b->clear,
		0,0,
		w->_w,w->_h);
	return;
}
void showBuffer(winInfo *w, bufInfo *b)
{
	XSetWindowBackgroundPixmap(display.dpy, w->win, b->buffer);
	XClearWindow(display.dpy, w->win);
	return;
}
void destroyBuffer(bufInfo *b)
{
	/* FIXME: Needs to destroy pixmap. */
	return;
}
/************************************************************************/
/*							Text functions								*/
/************************************************************************/
void createText(bufInfo *b,txtInfo *t)									/* Initiate text */
{
	char font[50];
	sprintf(font,"%s:pixelsize=%d",t->_font,t->_size);
	XRenderColor colortmp;												/* The colour struct */
	XGlyphInfo extents;
	t->font = XftFontOpenName(
		display.dpy,
		display._screen,
		font);
	colortmp.red = t->_r;
	colortmp.green = t->_g;
	colortmp.blue = t->_b;
	colortmp.alpha = t->_a;
	XftColorAllocValue(display.dpy,
		DefaultVisual(display.dpy, 0),
		DefaultColormap(display.dpy, 0),
		&colortmp, &t->fgColour);
	t->draw = XftDrawCreate(display.dpy,
		b->buffer,
		DefaultVisual(display.dpy, 0),
		DefaultColormap(display.dpy, 0));
	XftTextExtents8(display.dpy, t->font, t->_text,strlen(t->_text),&extents);
	t->_w = extents.width;
	t->_h = t->_size;
	if (t->_h > b->_hTotal) {
		b->_hTotal = t->_h;
	}
	return;																/* Return to main */
}
void addText(winInfo *w,bufInfo *b, txtInfo *t)
{
	int y = b->_y+(t->_h);												/* Claculate the Y position */
	XftDrawString8(
		t->draw,
		&t->fgColour,
		t->font,
		b->_x+t->_space,
		y,
		t->_text,
		strlen(t->_text));
	b->_x += t->_w;														/* Increase the buffer X holder */
	b->_wTotal += t->_w+t->_space;
	return;
}
/************************************************************************/
/*							Image functions								*/
/************************************************************************/
void readImage(bufInfo *b,imgInfo *i)									/* Retrieve a image from a file */
{
	i->iData=Imlib_init(display.dpy);
	i->iFull=Imlib_load_image(i->iData,i->_name);
	i->_iw=i->iFull->rgb_width;
	i->_ih=i->iFull->rgb_height;
	Imlib_render(i->iData,i->iFull,i->_iw,i->_ih);
	i->p=Imlib_move_image(i->iData,i->iFull);
	i->m=Imlib_move_mask(i->iData,i->iFull);
	if (i->_h > b->_hTotal) {
		b->_hTotal = i->_h;
	}
	return;
}
void addImage(winInfo *w, bufInfo *b, imgInfo *i)
{
	int y = b->_y;														/* Claculate the Y position */	
	Imlib_paste_image(													/* Paste the image into the buffer */
		i->iData,
		i->iFull,
		b->buffer,
		b->_x+i->_space,
		y,
		i->_w,
		i->_h);
	b->_x += i->_w + i->_space;											/* Increase the buffer X holder */
	b->_wTotal += i->_w + i->_space;
	return;
}
void destroyImage(imgInfo *i)
{
	/* FIXME: This needs to clear the image from memory */
	return;
}
/************************************************************************/
/*					Timer/Delay functions								*/
/************************************************************************/
int timeDelay(int i)
{
	struct timeval timeout;
	if (i>0)
	{
		timeout.tv_usec = i % (unsigned long) 1000000;
		timeout.tv_sec = i / (unsigned long) 1000000;
		select(0, NULL, NULL, NULL, &timeout);
	}
	return (i>0 ? i : 0);
}
int timeDiff()
{
	gettimeofday(&delay.rt, NULL);
	delay._d = (1000000*(delay.rt.tv_sec-delay.st.tv_sec))+(delay.rt.tv_usec-delay.st.tv_usec);
	delay.st = delay.rt;
	return delay._d;
}
