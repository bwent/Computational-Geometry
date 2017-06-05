/* vis.cpp

Laura Toma

What it does:  The user can enter a polygon by clicking on the mouse.


*/


#include "vg.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <array>
#include <vector>

#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <vector>
#include <map>

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

int changeObstacle = 0 ;
vector<vector<point2D> > obstacles; /* global var for the obstacles */
vector<line2D> VG;
vector<line2D> SP;
vector<point2D> poly;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);

void initialize_arena();
void print_polygon(vector<point2D> poly);
void draw_polygon(vector<point2D> poly, GLfloat *c);
void fill_polygon(vector<point2D> poly, GLfloat *c);
void draw_circle(double x, double y, GLfloat *c);
void draw_line(line2D line, GLfloat *c);


/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
origin is the lower left corner */


/* global variables */
const int WINDOWSIZE = 750;


//coordinates of last mouse click
double mouse_x=-10, mouse_y=-10;
point2D start_point;
point2D end_point;
bool setting_source = true;
bool setting_dest = false;
bool done_init = false;
bool fill_polygons = false;

//initialized to a point outside the window

//when this is 1, then clicking the mouse results in those points being stored in poly
int poly_init_mode = 0;

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

void draw_line(line2D line, GLfloat *c) {
  glColor3fv(c);

  glBegin(GL_LINES);

  glVertex2f(line.a.x, line.a.y);
  glVertex2f(line.b.x, line.b.y);

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

    if (poly_init_mode ==1 && !setting_source && !setting_dest) {

      point2D clicked = {.x = mouse_x, .y = mouse_y};
      poly.push_back(clicked);


    } else {
      point2D clicked = {.x = mouse_x, .y = mouse_y};
      if (setting_source) {
        printf("source point: (%d %d)\n", (int)mouse_x, (int)mouse_y);
        start_point = clicked;
      }
      if (setting_dest) {
        printf("dest: %d %d\n", (int)mouse_x, (int)mouse_y);
        end_point = clicked;
      }
    }

  }

  glutPostRedisplay();
}




/* ****************************** */
/* initialize  polygon stored in global variable poly  */
void initialize_arena() {

  //clear the vector, in case something was there
  obstacles.clear();
  glutPostRedisplay();


  vector<point2D> poly1;
  int n = 8; //size of polygon
  double rad = 80;
  double step = 2 * M_PI / n;
  point2D p;
  for (int i=0; i<n; i++) {

    p.x = WINDOWSIZE/2 + rad * cos (i * step);
    p.y = WINDOWSIZE/3 + rad * sin (i * step);
    //insert the segment in the array of segments
    poly1.push_back(p);
  } //for i
  obstacles.push_back(poly1);

  vector<point2D> poly2;
  point2D p1 = {.x = WINDOWSIZE/6, .y = WINDOWSIZE/2};
  point2D p2 = {.x = WINDOWSIZE/6 + 100, .y = WINDOWSIZE/2 + 10};
  point2D p3 = {.x = WINDOWSIZE/6 - 10, .y = WINDOWSIZE/2 + 80};
  poly2.push_back(p1);
  poly2.push_back(p2);
  poly2.push_back(p3);
  obstacles.push_back(poly2);

  vector<point2D> poly3;
  point2D p5 = {.x = WINDOWSIZE - 100, .y = WINDOWSIZE/2};
  point2D p6 = {.x = WINDOWSIZE - 120, .y = WINDOWSIZE/2 + 100};
  point2D p7 = {.x = WINDOWSIZE/2 + 100, .y = WINDOWSIZE/2 + 80};
  point2D p8 = {.x  = WINDOWSIZE/2 + 50, .y = WINDOWSIZE/3 + 100};
  poly3.push_back(p5);
  poly3.push_back(p6);
  poly3.push_back(p7);
  poly3.push_back(p8);
  obstacles.push_back(poly3);

}

void reset() {
  fill_polygons = false;
  poly.clear();
  obstacles.clear();
  VG.clear();
  SP.clear();
  setting_source = false;
  setting_dest = false;
  done_init = false;

  start_point.x = -10; start_point.y = -10;
  end_point.x = -10; end_point.y = -10;
  mouse_x = -10;
  mouse_y = -10;


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
  //  glutIdleFunc(timerfunc);

  /* init GL */
  /* set background color black*/
  glClearColor(0.8, 0.8, 0.8, 0.8);
  /* here we can enable depth testing and double buffering and so
  on */
  /* Enable transparency */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glLineWidth(2);


  /* give control to event handler */
  glutMainLoop();
  return 0;
}


/* ****************************** */
/* draw the polygon */
void draw_polygon(vector<point2D> poly, GLfloat *c){

  int n = poly.size();
  if (n == 0) return;

  glColor3fv(c);

  int i;
  for (i=0; i<poly.size(); i++) {

    glBegin(GL_LINES);

    glVertex2f(poly[i].x, poly[i].y);
    glVertex2f(poly[(i+1)%n].x, poly[(i+1)%n].y);

    glEnd();
  }

}
// Called for errors in tessellation
void polygon_error(GLenum errno) {
  printf("ERROR!!!\n");
  printf("The error is %s\n", gluErrorString(errno));
}

// Called for intersections in tessellation
void combine(GLdouble coords[3], void* vertex_data[4], GLfloat weight[4],
             void** outData) {
  std::array<double, 3> point = { {coords[0], coords[1], coords[2]} };
  *outData = &point[0];
}

// fill the polygon!
void fill_polygon(vector<point2D> poly, GLfloat *c) {
  if (poly.size() == 0) return;

  // convert the structs to arrays for the OpenGL C API
  // std::vector<double*> points;
  vector<std::array<double, 3> > points;
  point2D p;
  for (int i = 0; i < poly.size(); ++i) {
    p = poly[i];
    std::array<double, 3> point = {{(double)p.x, (double)p.y, 0}};
    points.push_back(point);
  }

  // Tessellate and fill the polygon
  glColor3fv(c);
  GLUtesselator *tess = gluNewTess();
  gluTessCallback(tess, GLU_BEGIN, (GLvoid (*) ())glBegin);
  gluTessCallback(tess, GLU_VERTEX, (GLvoid (*) ())glVertex3dv);
  gluTessCallback(tess, GLU_END, (GLvoid (*) ())glEnd);
  gluTessCallback(tess, GLU_ERROR, (GLvoid (*) ())polygon_error);
  gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*) ())combine);

  gluBeginPolygon(tess);
  for (size_t i = 0; i < points.size(); i++) {
    gluTessVertex(tess, &points[i][0], &points[i][0]);
  }
  gluEndPolygon(tess);
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

  /* draws the user-drawn polygon */
  if (poly_init_mode == 1 && !poly.empty()) {
    draw_polygon(poly, cyan);
  }

  //printf("num obstacles is %d\n", obstacles.size());
  for (int i = 0; i < obstacles.size(); ++i) {
    draw_polygon(obstacles[i], yellow);
    if (fill_polygons) {
      fill_polygon(obstacles[i], yellow);
    }
  }


  //now we draw in our local coordinate system (0,0) to
  //(WINSIZE,WINSIZE), with the origin in the lower left corner.
  // for (int i = 0; i < obstacles.size(); ++i) {
  //     draw_polygon(obstacles[i], yellow);
  // }
  // draw the visibility graph once it's computed
  if (!VG.empty()) {
    for (int i = 0; i < VG.size(); ++i) {
      draw_line(VG[i], cyan);
    }
  }
  if (!SP.empty()) {
    for (int i = 0; i < SP.size(); ++i) {
      draw_line(SP[i], magenta);
    }
  }

  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse
  //handler function
  draw_circle(mouse_x, mouse_y, white);

  draw_circle(start_point.x, start_point.y, blue);
  draw_circle(end_point.x, end_point.y, green);


  /* execute the drawing commands */
  glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
    case 'q':
    exit(0);
    break;

    case 'i':
      obstacles.clear();
      initialize_arena();
      glutPostRedisplay();
      break;

    //expected behaviour: press 's', then click on the points you
    //want, and press 'e' when you're done. the points will be saved
    //in global 'poly'
    case 'c':
    reset();

    poly_init_mode = 1;
    glutPostRedisplay();
    break;

    case 'd':
    //start re-initializeing polygon
    poly_init_mode = 1;
    setting_source = false;
    mouse_x = mouse_y = 0;
    if (!poly.empty()) {
      obstacles.push_back(poly);
      poly.clear();
    }

    glutPostRedisplay();
    break;

    case 's':
    //mouse_x = mouse_y=0;
    setting_source = true;
    //poly_init_mode = 1;
    SP.clear();
    VG.clear();
    glutPostRedisplay();
    break;

    case 'f':
    //mouse_x = mouse_y=0;
    setting_source = false;
    setting_dest = true;

    //  poly_init_mode = 1;
    SP.clear();
    VG.clear();
    glutPostRedisplay();
    break;

    case 'v':
    SP.clear();
    if (setting_dest) {
      setting_dest = false;
      done_init = true;
    }

    VG.clear();
    if (!obstacles.empty() && done_init) {
      printf("number of obstacles: %lu\n", obstacles.size());
      fill_polygons = true;
      VG = compute_visibility_graph(start_point, end_point, obstacles);
      printf("VG size: %lu\n", VG.size());
    }
    glutPostRedisplay();
    break;

    case 'p':
    SP.clear();
    if (VG.empty()) {
      printf("must compute the visibility graph first . . . press v\n");
    } else {
      SP = dijkstra(start_point, end_point);
      glutPostRedisplay();
    }

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
