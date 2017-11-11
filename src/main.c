#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"

/* X variables */
Display *dis;
int screen;
Window win;
GC gc;

/* Other global variables */
walkLight_t w1 = {
  .state = stay,
  .redLight = 1,
  .greenLight = 0
};

walkLight_t w2 = {
  .state = stay,
  .redLight = 1,
  .greenLight = 0
};

carLight_t c1 = {
  .state = stop,
  .redLight = 1,
  .yellowLight = 0,
  .greenLight = 0,
  .nextCarLight = NULL,
  .walkLight = &w2
};

carLight_t c2 = {
  .state = stop,
  .redLight = 1,
  .yellowLight = 0,
  .greenLight = 0,
  .nextCarLight = NULL,
  .walkLight = &w1
};

carLight_t* curCar = &c1;

/* Function declarations */
void init_x();
void initVars();
void close_x();
void redraw();

/* Function definitions */
unsigned long GetColor( Display* dis, char* color_name ) {
  Colormap cmap;
  XColor near_color, true_color;

  cmap = DefaultColormap( dis, 0 );
  XAllocNamedColor( dis, cmap, color_name, &near_color, &true_color );
  return( near_color.pixel );
}

void init_x() {
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

void close_x() {
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

void draw_lights() {
  drawCarLight(c1, 40, 40);
  drawCarLight(c2, 260, 40);
  drawWalkLight(w1, 100, 80);
  drawWalkLight(w2, 320, 80);
}

void changeState() {
  walkLight_t* w = curCar->walkLight;
  switch (curCar->state) {
  case stop:
    curCar->redLight = 1;
    curCar->yellowLight = 0;
    curCar = curCar->nextCarLight;
    curCar->state = drive_soon;
    w->state = stay;
    break;
  case drive_soon:
    curCar->yellowLight = 1;
    curCar->state = drive;
    w->state = go;
    break;
  case drive:
    curCar->redLight = 0;
    curCar->yellowLight = 0;
    curCar->greenLight = 1;
    curCar->state = stop_soon;
    break;
  case stop_soon:
    curCar->yellowLight = 1;
    curCar->greenLight = 0;
    curCar->state = stop;
    break;
  }

  switch (w->state) {
  case stay:
    w->redLight = 1;
    w->greenLight = 0;
    break;
  case go:
    w->redLight = 0;
    w->greenLight = 1;
    break;
  }
}

void redraw() {
  XClearWindow(dis, win);
  draw_lights();
};

int main() {
  XEvent event;
  KeySym key;
  char text[255];

  init_x();
  c1.nextCarLight = &c2;
  c2.nextCarLight = &c1;

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
      changeState();
      redraw();
    }
  }
  return 0;
}
