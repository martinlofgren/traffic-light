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
light_t wg1 = {
  .type = walk,
  .w = &w1,
  .pos = { .x = 120, .y = 50}
};
carLight_t c1 = {
  .state = stop,
  .redLight = 1,
  .yellowLight = 0,
  .greenLight = 0,
  .nextCarLight = NULL,
  .walkLight = &w1
};
light_t wc1 = {
  .type = car,
  .c = &c1,
  .pos = { .x = 50, .y = 50 }
};

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
			    GetColor(dis, "black"), GetColor(dis, "white"));
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

void drawCarLight(carLight_t car) {
  XSetForeground(dis, gc, GetColor(dis, "black"));
  XFillRectangle(dis, win, gc, 40, 40, 50, 130);
  XSetForeground(dis, gc, GetColor(dis, (car.redLight) ? "red" : "red4"));
  XFillArc(dis, win, gc, 50, 50, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (car.yellowLight) ? "yellow" : "yellow4"));
  XFillArc(dis, win, gc, 50, 90, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (car.greenLight) ? "green" : "green4"));
  XFillArc(dis, win, gc, 50, 130, 30, 30, 0, 360*64);
}

void drawWalkLight(walkLight_t w) {
  XSetForeground(dis, gc, GetColor(dis, "black"));
  XFillRectangle(dis, win, gc, 100, 80, 50, 90);
  XSetForeground(dis, gc, GetColor(dis, (w.redLight) ? "red" : "red4"));
  XFillArc(dis, win, gc, 110, 90, 30, 30, 0, 360*64);
  XSetForeground(dis, gc, GetColor(dis, (w.greenLight) ? "green" : "green4"));
  XFillArc(dis, win, gc, 110, 130, 30, 30, 0, 360*64);
}

void draw_lights() {
  drawCarLight(c1);
  //  drawWalkLight(w1);
}

void changeState() {
  static uint8_t holdRedLight = 0;
  walkLight_t* w = c1.walkLight;

  switch (c1.state) {
  case stop:
    c1.redLight = 1;
    c1.yellowLight = 0;
    if (w->state == stay && !holdRedLight) {
      holdRedLight = 1;
    }
    else if (w->state == stay && holdRedLight) {
      w->state = go;
    }
    else {
      w->state = stay;
      c1.state = drive_soon;
      holdRedLight = 0;
    }
    break;
  case drive_soon:
    c1.yellowLight = 1;
    c1.state = drive;
    break;
  case drive:
    c1.redLight = 0;
    c1.yellowLight = 0;
    c1.greenLight = 1;
    c1.state = stop_soon;
    break;
  case stop_soon:
    c1.yellowLight = 1;
    c1.greenLight = 0;
    c1.state = stop;
    break;
  }

  switch (w->state) {
  case stay:
    w1.redLight = 1;
    w1.greenLight = 0;
    break;
  case go:
    w1.redLight = 0;
    w1.greenLight = 1;
    break;
  }
}

void redraw() {
  XClearWindow(dis, win);
  draw_lights();
};

int main() {
  XEvent event;		/* the XEvent declaration !!! */
  KeySym key;		/* a dealie-bob to handle KeyPress Events */	
  char text[255];		/* a char buffer for KeyPress Events */

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
      changeState();
      redraw();
    }
  }
  return 0;
}
