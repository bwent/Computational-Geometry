/* vis.cpp

Laura Toma

What it does:  The user can enter a polygon by clicking on the mouse.


*/


#include "planner.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <array>
#include <vector>
#include <ctime>

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
vector<state> SP;
int i = 0;
//state*** EO;
vector<point2D> poly;
robot2D robot;
bool animate = false;
bool animation_done = false;


/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);

void timer();
void initialize_arena();
void initialize_test1();
void print_polygon(vector<point2D> poly);
void draw_polygon(vector<point2D> poly, GLfloat *c);
void draw_circle(double x, double y, GLfloat *c);
void draw_robot(double x, double y, int theta, GLfloat *c);
//void draw_and_animate_robot(vector<state> SP);
void draw_line(line2D line, GLfloat *c);


/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
origin is the lower left corner */

//coordinates of last mouse click
double mouse_x=-100, mouse_y=-100;
point2D start_point = {.x = -100, .y = -100};
point2D end_point = {.x = -100, .y = -100};
bool setting_source = true;
bool setting_dest = false;
bool done_init = false;

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

void draw_robot(double x, double y, int theta, GLfloat *c){

  glColor3fv(c);

    int len = ROBOT_HEIGHT;
    int width = ROBOT_WIDTH;

    point2D a = {0,0};
    point2D b = {width,0};
    point2D c1 = {width,len};
    point2D d = {0,len};

    robot.p0.x = a.x * cos(theta) - a.y * sin(theta);
    robot.p0.y = a.x * sin(theta) + a.y * cos(theta);
    robot.p1.x = b.x * cos(theta) - b.y * sin(theta);
    robot.p1.y = b.x * sin(theta) + b.y * cos(theta);
    robot.p2.x = c1.x * cos(theta) - c1.y * sin(theta);
    robot.p2.y = c1.x * sin(theta) + c1.y * cos(theta);
    robot.p3.x = d.x * cos(theta) - d.y * sin(theta);
    robot.p3.y = d.x * sin(theta) + d.y * cos(theta);

    robot.p0.x += x;
    robot.p0.y += y;
    robot.p1.x += x;
    robot.p1.y += y;
    robot.p2.x += x;
    robot.p2.y += y;
    robot.p3.x += x;
    robot.p3.y += y;


  glBegin(GL_POLYGON);

  glVertex2f(robot.p0.x, robot.p0.y);
  glVertex2f(robot.p1.x, robot.p1.y);
  glVertex2f(robot.p2.x, robot.p2.y);
  glVertex2f(robot.p3.x, robot.p3.y);


  glEnd();
}

// void draw_and_animate_robot(vector<state> SP) {
//   for (int i = 0; i < SP.size() -1; i++) {
//     line2D line;
//     line.a.x = SP[SP.size() -i].x;
//     line.a.y = SP[SP.size() -i].y;
//     line.b.x = SP[SP.size() -i -1].x;
//     line.b.y = SP[SP.size() -i -1].y;
//     draw_line(line, magenta);
//   }
//   animate = true;
//
// }

/* idle timer func called every frame to handle animation functionality */
void timer() {

  if (animate && !SP.empty()) {

    if (i == 0) {
      animate = false;
      animation_done = true;
      i = SP.size() - 1;
      return;
    }
    robot.p0.x = SP[i].x;
    robot.p0.y = SP[i].y;
    robot.theta = SP[i].theta;

    i--;
    animation_done = false;

    glutPostRedisplay();

  }
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

        robot.p0.x = mouse_x;
        robot.p0.y = mouse_y;
        robot.theta = 0;

      }
      if (setting_dest) {
        printf("dest: %d %d\n", (int)mouse_x, (int)mouse_y);
        end_point = clicked;
        done_init = true;
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
  int n = 4; //size of polygon
  double rad = 50;
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
  glutIdleFunc(timer);

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

// // Draw the polygon!
// void draw_polygon(std::vector<point2D> poly, GLfloat *c) {
//   if (poly.size() == 0) return;
//
//   // convert the structs to arrays for the OpenGL C API
//   // std::vector<double*> points;
//   std::vector<std::array<double, 3> > points;
//   point2D p;
//   for (int i = 0; i < poly.size(); ++i) {
//     p = poly[i];
//     std::array<double, 3> point = {{(double)p.x, (double)p.y, 0}};
//     points.push_back(point);
//   }
//
//   // Tessellate and fill the polygon
//   glColor3fv(c);
//   GLUtesselator *tess = gluNewTess();
//   gluTessCallback(tess, GLU_BEGIN, (GLvoid (*) ())glBegin);
//   gluTessCallback(tess, GLU_VERTEX, (GLvoid (*) ())glVertex3dv);
//   gluTessCallback(tess, GLU_END, (GLvoid (*) ())glEnd);
//   gluTessCallback(tess, GLU_ERROR, (GLvoid (*) ())polygon_error);
//   gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*) ())combine);
//
//   gluBeginPolygon(tess);
//   for (size_t i = 0; i < points.size(); i++) {
//     gluTessVertex(tess, &points[i][0], &points[i][0]);
//   }
//   gluEndPolygon(tess);
// }


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
    draw_polygon(poly, yellow);
  }

  //printf("num obstacles is %d\n", obstacles.size());
  for (int i = 0; i < obstacles.size(); ++i) {
    draw_polygon(obstacles[i], yellow);
  }


  if (!SP.empty()) {
    int n = SP.size();
    for (int i = 0; i < SP.size()-1; ++i) {
      line2D line;

      line.a.x = SP[i].x;
      line.a.y = SP[i].y;
      line.b.x = SP[(i+1)].x;
      line.b.y = SP[(i+1)].y;
      draw_line(line, magenta);
    }
  }

    for (int i = 0; i < obstacles.size(); ++i)
        draw_polygon(obstacles[i], cyan);

  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse
  //handler function


  if (done_init) {

    draw_robot(robot.p0.x, robot.p0.y, robot.theta, blue);
  } else {

    draw_robot(start_point.x, start_point.y, 0, blue);
  }

  if (setting_dest) {
    draw_circle(end_point.x, end_point.y, green);
  } else if (!setting_source){
    draw_circle(mouse_x, mouse_y, white);
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

    case 'i':
      obstacles.clear();
      initialize_arena();
      glutPostRedisplay();
      break;


    //expected behaviour: press 's', then click on the points you
    //want, and press 'e' when you're done. the points will be saved
    //in global 'poly'
    case 'c':
    SP.clear();
    poly.clear();
    obstacles.clear();
    poly_init_mode = 1;
    setting_source = false;
    setting_dest = false;
    done_init = false;
    glutPostRedisplay();
    break;

    case 'd':
    //start re-initializeing polygon
    poly_init_mode = 1;
    setting_source = false;
    mouse_x = mouse_y = 0;
    if (!poly.empty()) {
      printf("adding next polygon: %lu\n", obstacles.size());
      obstacles.push_back(poly);
      poly.clear();
    }

    glutPostRedisplay();
    break;


    case 's':
    mouse_x = mouse_y=-10;
    setting_source = true;
    poly_init_mode = 0;
    SP.clear();
    glutPostRedisplay();
    break;

    case 'f':
    //mouse_x = mouse_y=0;
    setting_source = false;
    setting_dest = true;

    //  poly_init_mode = 1;
    SP.clear();
    glutPostRedisplay();
    break;

    case 'p':
    SP.clear();
    animate = false;

    if (setting_source) {
      setting_source = false;
    }
    poly_init_mode = 0;
    if (setting_dest) {
      setting_dest = false;
      done_init = true;
    }

    if (!obstacles.empty()) {
      printf("number of obstacles: %lu\n", obstacles.size());
    }

    SP.clear();
    clock_t start;
    double duration;
    printf("computing shortest path...\n");
    start = clock();
    SP = a_star(start_point, end_point, obstacles);
    duration = (clock() - start) / (double) CLOCKS_PER_SEC;
    printf("done! shortest path computed in %fs.\n", duration);

    glutPostRedisplay();
    break;

    case 'a':

    animate = !animate;

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
