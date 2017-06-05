/* vis.cpp

Laura Toma

What it does:  The user can enter a polygon by clicking on the mouse.


*/


#include "guard.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <vector>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;




/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);
void timer();
void animateDisplay();
void erasePoly();
void initialize_polygon();
void print_polygon(vector<point2D> poly);
void draw_polygon(vector<point2D> poly, GLfloat *c, int fillmode);


/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
origin is the lower left corner */


/* global variables */
const int WINDOWSIZE = 750;


//the current polygon
vector<point2D>  poly;
vector<point2D>  vis;

int speed = 10;
bool direction = true;

//coordinates of last mouse click
double mouse_x=-10, mouse_y=-10;
point2D G; /* global var for the guard */
//initialized to a point outside the window

//when this is 1, then clicking the mouse results in those points being stored in poly
int poly_init_mode = 0;

bool animate = false;



double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}


double x_slope = random() % 5 - 2;
double y_slope = random() % 5 - 2;

void draw_circle(double x, double y, GLfloat *c){
  glColor3fv(c);
  int precision = 100;
  double r = 4;
  double theta = 0;
  glBegin(GL_POLYGON);
  for(int i = 0; i < precision; i++){
    theta = i * 2 * M_PI/precision;
    glVertex2f(x + r*cos(theta), y + r*sin(theta));
  }
  glEnd();
}



/*
Usage

void glutMouseFunc(void (*func)(int button, int state, int x, int y));

Description

glutMouseFunc sets the mouse callback for the current window. When a
user presses and releases mouse buttons in the window, each press and
each release generates a mouse callback. The button parameter is one
of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON. For
systems with only two mouse buttons, it may not be possible to
generate GLUT_MIDDLE_BUTTON callback. For systems with a single mouse
button, it may be possible to generate only a GLUT_LEFT_BUTTON
callback. The state parameter is either GLUT_UP or GLUT_DOWN
indicating whether the callback was due to a release or press
respectively. The x and y callback parameters indicate the window
relative coordinates when the mouse button state changed. If a
GLUT_DOWN callback for a specific button is triggered, the program can
assume a GLUT_UP callback for the same button will be generated
(assuming the window still has a mouse callback registered) when the
mouse button is released even if the mouse has moved outside the
window.
*/

void mousepress(int button, int state, int x, int y) {


  if(state == GLUT_DOWN) {

    mouse_x = x;
    mouse_y = y;
    //(x,y) are in wndow coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    mouse_y = WINDOWSIZE - mouse_y;

    printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);

    if (poly_init_mode ==1) {
      point2D clicked = {.x = mouse_x, .y = mouse_y};
      poly.push_back(clicked);
    } else {
      point2D clicked = {.x = mouse_x, .y = mouse_y};
      vis.clear();
      vis = findVisiblePoly(poly, clicked);
    }


  }

  glutPostRedisplay();
}




/* ****************************** */
/* initialize  polygon stored in global variable poly  */
void initialize_polygon() {

  //clear the vector, in case something was there
  poly.clear();

  int n = 10; //size of polygon
  double rad = 100;
  double step = 2 * M_PI / n;
  point2D p;
  for (int i=0; i<n; i++) {

    p.x = WINDOWSIZE/2 + rad * cos (i * step);
    p.y = WINDOWSIZE/2 + rad * sin (i * step);


    //insert the segment in the array of segments
    poly.push_back (p);
  } //for i
}



/* ************************************************** */
void print_polygon(vector<point2D> poly) {

  for (unsigned int i=0; i<poly.size()-1; i++) {
    printf("edge %d: [(%d,%d), (%d,%d)]\n",
    i, poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  }
  //print last edge from last point to first point
  int last = poly.size()-1;
  printf("edge %d: [(%d,%d), (%d,%d)]\n",
  last, poly[last].x, poly[last].y, poly[0].x, poly[0].y);

}




/* ****************************** */
int main(int argc, char** argv) {

  initialize_polygon();
  print_polygon(poly);


  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display);
  glutKeyboardFunc(keypress);

  glutMouseFunc(mousepress);
  glutIdleFunc(timer);

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);
  /* here we can enable depth testing and double buffering and so
  on */
  /* Enable transparency */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  /* give control to event handler */
  glutMainLoop();
  return 0;
}


/* ****************************** */
/* draw the polygon */
void draw_polygon(vector<point2D> poly, GLfloat *c, int fillmode){
  int n = poly.size();
  if (n == 0) return;

  //set color
  if (fillmode == 1) {
    glColor4f(c[0], c[1], c[2], 0.5);
  } else {
    glColor3fv(c);
  }

  int i;
  for (i=0; i<poly.size(); i++) {
    if (fillmode == 1) {
      glBegin(GL_POLYGON);
    } else {
      glBegin(GL_LINES);
    }
    glVertex2f(poly[i].x, poly[i].y);
    glVertex2f(poly[(i+1)%n].x, poly[(i+1)%n].y);
    if (fillmode == 1) {
      glVertex2f(G.x, G.y);
    }
    glEnd();
  }

  //glFlush();
}





/* ****************************** */
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); //clear the matrix


  /* The default GL window is [-1,1]x[-1,1] with the origin in the
  center.

  Our system of coordinates (in which we generate our points) is
  (0,0) to (WINSIZE,WINSIZE), with the origin in the lower left
  corner.

  We need to map the points to [-1,1] x [-1,1]

  Assume we are the local coordinate system.

  First we scale down to [0,2] x [0,2] */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
  /* Then we translate so the local origin goes in the middle of teh
  window to (-WINDOWSIZE/2, -WINDOWSIZE/2) */
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);

  //now we draw in our local coordinate system (0,0) to
  //(WINSIZE,WINSIZE), with the origin in the lower left corner.
  draw_polygon(poly, yellow, 0);



  for (int i = 0; i < vis.size(); ++i) {
    draw_circle(vis[i].x, vis[i].y, cyan);
  }
  draw_polygon(vis, cyan, 1);

  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse
  //handler function
  if (!animate) {
    draw_circle(mouse_x, mouse_y, blue);
  } else {
    draw_circle(G.x, G.y, blue);
  }

  /* execute the drawing commands */
  glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
    case 'q':
    exit(0);
    break;

    //expected behaviour: press 's', then click on the points you
    //want, and press 'e' when you're done. the points will be saved
    //in global 'poly'
    case 's':
    //start re-initializeing polygon
    animate = false;
    poly.clear();
    vis.clear();
    mouse_x = mouse_y=0;
    poly_init_mode = 1;
    glutPostRedisplay();
    break;

    /* guard the polygon */
    case 'g':
    poly_init_mode=0;
    animate = false;
    glutPostRedisplay();
    break;

    /* start animation */
    case 'a':
    animate = !animate;
    glutPostRedisplay();
    break;

    /* undo the last line of polygon that was drawn */
    case 'z':
    poly.erase(poly.end()-1);
    glutPostRedisplay();
    break;
  }

}


/* Handler for window re-size event. Called back when the window first appears and
whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer

  // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
  glLoadIdentity();             // Reset
  gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}


/* idle timer func called every frame to handle animation functionality */
void timer() {

  if (animate) {

    if (isInside(G, poly)) {
      glutPostRedisplay();

      /* if we're inside, add the new slope onto the guard point */
      G.x += x_slope;
      G.y += y_slope;

      vis = findVisiblePoly(poly, G); // cmopute visible polygon as we animate

    } else {

      while(!isInside(G, poly) || (x_slope == 0 && y_slope == 0)) {
        /* first, reverse the slope direction to get us back in */
        G.x -= x_slope;
        G.y -= y_slope;

        /* choose a new random slope while we're outside the guard */
        x_slope = random() % 5 - 2;
        y_slope = random() % 5 - 2;
        G.x += x_slope;
        G.y += y_slope;

      }



    }
  } else {
    /* not animating; simply set the guard point to the user clicked point */
    G.x = mouse_x;
    G.y = mouse_y;
  }


}
