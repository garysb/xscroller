/* test.c - Just to see if all in library is working */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "libppx.h"

#define MAXWINS 10	// maximum number of window threads

void displayWindow(xmlDocPtr doc){
	winInfo window;
	xmlChar *key;
	/*bufInfo buf;*/
	window = parseWindow(doc);
	/*buf._x = 0;*/
	createWindow(&window);
	nameWindow(&window);
	XFlush(display.dpy);					/* Flush the display */
	/*createBuffer(&window,&buf);*/				/* Create buffer */
	/*clearBuffer(&window,&buf);*/
	/*XFlush(display.dpy);*/					/* Flush the display */
	while (window.xmlCur != NULL) {
		if ((!xmlStrcmp(window.xmlCur->name, (const xmlChar *)"object"))) {
			key = xmlNodeListGetString(doc, window.xmlCur->xmlChildrenNode, 1);
			parseObjects(doc,&window,window.xmlCur);
			xmlFree(key);
		}
		window.xmlCur = window.xmlCur->next;
	}
	/*parseEase(doc,&window,&buf,window.xmlCur);*/
	destroyWindow(&window);
	/*pthread_exit(0);*/
	return;
}

int main() {
	xmlChar *key;
	xmlInfo xmlDocu;
	/* Name xml file */
	char xmlFile[50];
	strcpy(xmlFile,"test.xml");
	xmlDocu = loadXml(xmlFile);
	display = parseDisplay(&xmlDocu);
	createDisplay();
	/* Need to loop through display and create windows */
	while (display.xmlCur != NULL) {
		if ((!xmlStrcmp(display.xmlCur->name, (const xmlChar *)"window"))) {
			key = xmlNodeListGetString(xmlDocu.doc, display.xmlCur->xmlChildrenNode, 1);
			 /*pthread_create(&thread[i],
			 	pthread_attr_default,
				(void *) &displayWindow, (void) doc);*/
			displayWindow(xmlDocu.doc);
			xmlFree(key);
		}
		display.xmlCur = display.xmlCur->next;
	}
	xmlFreeDoc(xmlDocu.doc);
	return 0;										/* Exit test */
}
