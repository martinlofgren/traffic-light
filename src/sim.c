/*
 * File: sim.c
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "sim.h"

/* X variables */
Display *dis;
int screen;
Window win;
GC gc;

/* Application global variables */
voidFunction irqFn;

/* Local function declarations */
void init_x(void);
void initVars(void);
void close_x(void);
void redraw(void);

/* Function definitions */
unsigned long GetColor(Display* dis, char* color_name) {
  Colormap cmap;
  XColor near_color, true_color;

  cmap = DefaultColormap(dis, 0);
  XAllocNamedColor(dis, cmap, color_name, &near_color, &true_color);
  return(near_color.pixel);
}

void init_x(void) {
  dis = XOpenDisplay((char *)0);
  screen = DefaultScreen(dis);
  win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 300, 300, 5,
			    GetColor(dis, "black"), GetColor(dis, "grey20"));
  XSetStandardProperties(dis, win, "Trafic lights", "Hi", None, NULL, 0, NULL);
  XSelectInput(dis, win, ExposureMask|KeyPressMask);
  gc=XCreateGC(dis, win, 0,0);        
  XSetBackground(dis, gc, GetColor(dis, "white"));
  XSetForeground(dis, gc, GetColor(dis, "black"));
  XClearWindow(dis, win);
  XMapRaised(dis, win);
}

void close_x(void) {
  XFreeGC(dis, gc);
  XDestroyWindow(dis, win);
  XCloseDisplay(dis);	
  exit(1);				
};

void drawCarLight(carLight_t car, int x, int y) {
  XSetForeground(dis, gc, GetColor(dis, "black"));
  XFillRectangle(dis, win, gc, x, y, 50, 130);
  XSetForeground(dis, gc, GetColor(dis, (car.redLight) ? "red" : "red4"));
  XFillArc(dis, win, gc, x + 10, y + 10, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (car.yellowLight) ? "yellow" : "orange4"));
  XFillArc(dis, win, gc, x + 10, y + 50, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (car.greenLight) ? "green" : "DarkGreen"));
  XFillArc(dis, win, gc, x + 10, y + 90, 30, 30, 0, 360*64);
}

void drawWalkLight(walkLight_t w, int x, int y) {
  XSetForeground(dis, gc, GetColor(dis, "black"));
  XFillRectangle(dis, win, gc, x, y, 50, 90);
  XSetForeground(dis, gc, GetColor(dis, (w.redLight) ? "red" : "red4"));
  XFillArc(dis, win, gc, x + 10, y + 10, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (w.greenLight) ? "green" : "DarkGreen"));
  XFillArc(dis, win, gc, x + 10, y + 50, 30, 30, 0, 360*64);
}

void draw_lights(void) {
  drawCarLight(c1, 40, 40);
  drawCarLight(c2, 260, 40);
  drawWalkLight(w1, 100, 80);
  drawWalkLight(w2, 320, 80);
}

void redraw(void) {
  XClearWindow(dis, win);
  draw_lights();
};

void irqRegister(int irqNum, voidFunction fn) {
  irqFn = fn;
}

void run(void) {
  XEvent event;
  KeySym key;
  char text[255];

  init_x();
  while(1) {		
    XNextEvent(dis, &event);
	
    if (event.type == Expose && event.xexpose.count == 0) {
      redraw();
    }
    if (event.type == KeyPress&&
	XLookupString(&event.xkey, text, 255, &key,0) == 1) {
      if (text[0] == 'q') {
	close_x();
      }
      irqFn();
      redraw();
    }
  }
}
