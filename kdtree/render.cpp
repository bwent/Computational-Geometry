/*  render.cpp
*
*  visualization tool for rendering a kd tree in 2D space
*  and coloring it to look like a Mondrian painting.
*
*
*  created by bridget went and claire mccarthy 3/18/17
*  last modified 3/28/17
*
* cs3250: computational geometry, spring 2017
* laura toma
*
*/

//#include "rtimer.h"
#include "kd.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};




/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void init();


/* size of a point */
const int POINT_SIZE  = 3.0f;

//the array of n points
Point2D*  points = NULL;
int n;

// build the kd-tree created with the points
KdTree *tree = new KdTree();


/* ****************************** */
/* print the array of points stored in global variable points[]*/
void print_points() {
  assert(points);
  int i;
  printf("points: \n");
  for (i=0; i<n; i++) {
    printf("[%f,%f]\n", points[i].x, points[i].y);
  }
  printf("\n");
}


/* ****************************** */
/* initialize  the array of points stored in global variable points[] with random points */
void initialize_points_random() {
  //re-initialize points
  if (points) {
    free(points);
  }
  //allocate points
  points = (Point2D *)malloc(n * sizeof(Point2D));
  assert(points);

  int i;
  for (i=0; i<n; i++) {
    points[i].x = (int)(.1*WINDOWSIZE)/2 + rand() % ((int)(.9*WINDOWSIZE));
    points[i].y =  (int)(.1*WINDOWSIZE)/2 + rand() % ((int)(.9*WINDOWSIZE));
  }
}
// Jack Ward
// Initialize the `points` vector to a grid.
void initialize_points_grid() {
  //re-initialize points
  if (points) {
    free(points);
  }
  // clear the vector
  points = (Point2D *)malloc(n * sizeof(Point2D));
  assert(points);

  double window  = (double) WINDOWSIZE;
  double padding = window / 8;
  double width   = window - 2 * padding;
  int    side    = sqrt(n);
  double spacing = width / (double) side;
  int i = 0;
  for (int row = 0; row < side; row++) {
    for (int col = 0; col < side; col++) {
                 // col * spacing + padding
      double x = fma(col, spacing, padding);
                 // row * spacing + padding
      double y = fma(row, spacing, padding);

      points[i].x = x;
      points[i].y = y;
      i++;
    }
  }
  //free current tree
  if (tree) {
    delete tree;
  }

  tree = new KdTree(points, n);

  // print the tree
  tree->kd_print();
}

// Bo Bleckel & Henry Daniels Koch
// Initialize the points in a downward sloping diagonal
// Works best with n <= 100
void initialize_points_diag() {
    //re-initialize points
    if (points) {
      free(points);
    }
    // clear the vector
    points = (Point2D *)malloc(n * sizeof(Point2D));
    assert(points);


    for (int i = 0; i < n; i++) {
        double x = i * (WINDOWSIZE/n);
        double y = (n-i) * (WINDOWSIZE/n);
        points[i].x = x;
        points[i].y = y;

    }
    //free current tree
    if (tree) {
      delete tree;
    }

    tree = new KdTree(points, n);

    // print the tree
    tree->kd_print();
}

//Erik and Grace code
void initialize_points_circle(){
  //re-initialize points
  if (points) {
    free(points);
  }
  // clear the vector
  points = (Point2D *)malloc(n * sizeof(Point2D));
  assert(points);

	double radius = WINDOWSIZE / 5;
	Point2D p;
	for (int i = 0; i < n; i++){
		p.x = radius * cos (2*M_PI / n * i) + WINDOWSIZE / 2;
		p.y = radius * sin (2*M_PI / n * i) + WINDOWSIZE / 2;
		points[i] = p;
	}
  //free current tree
  if (tree) {
    delete tree;
  }

  tree = new KdTree(points, n);

  // print the tree
  tree->kd_print();
}


// spiral test case created by Bridget and Claire
void initialize_points_spiral() {
  //re-initialize points
  if (points) {
    free(points);
  }

  // clean up the array of points
  points = (Point2D *)malloc(n * sizeof(Point2D));
  assert(points);

  // make radius half of the screen size
  double r = (double)(WINDOWSIZE / 2);

  Point2D p;
	for (int i = 0; i < n; i++){
    double theta = (2*M_PI*i)/ n; //the current angle
    p.x = (r/n) * i * cos(theta) + WINDOWSIZE/2;
    p.y = 1.2 * (r/n) * i * sin(theta) + WINDOWSIZE/2;
    points[i] = p;
	}

  //free current tree
  if (tree) {
    delete tree;
  }

  tree = new KdTree(points, n);

  // print the tree
  tree->kd_print();
}



/* ****************************** */
int main(int argc, char** argv) {

  // read number of points from user
  if (argc!=2) {
    printf("usage: kdtree <nbPoints>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  printf("you entered n=%d\n", n);
  assert(n >0);

  //initialize the points and build kdtree
  init();

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display);
  glutKeyboardFunc(keypress);

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);

  /* circular points */
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPointSize(POINT_SIZE);

  /* give control to event handler */
  glutMainLoop();
  return 0;
}




/* ****************************** */
/* draws a single point */
void draw_point(Point2D point)
{
  glColor3fv(cyan);

  glBegin(GL_POINTS);
  glVertex2f(point.x, point.y);
  glEnd();
}


/* ****************************** */
/* draws a line between two points */
void draw_line(Point2D p1, Point2D p2)
{
  glColor3fv(black);
  glLineWidth(3.0f);

  glBegin(GL_LINES);
  glVertex2f(p1.x, p1.y);
  glVertex2f(p2.x, p2.y);
  glEnd();
}

/* draws a rectangle bounded by (xmin, ymin) and (xmax, ymax) */
void draw_rect(double xMin, double yMin, double xMax, double yMax)
{
  // randomly generate an index
  int c = rand() % 5;

  switch (c) {
    case 0:
    glColor3fv(white);
    break;
    case 1:
    glColor3fv(white);
    break;
    case 2:
    glColor3fv(yellow);
    break;
    case 3:
    glColor3fv(blue);
    break;
    case 4:
    glColor3fv(red);
    break;
    default:
    break;
  };


  /* draw the rect with the appropriate color */
  glBegin(GL_POLYGON);

  glVertex2f(xMin, yMin);
  glVertex2f(xMax, yMin);
  glVertex2f(xMax, yMax);
  glVertex2f(xMin, yMax);

  glEnd();
  glFlush();

}



/* ****************************** */
/* draw the array of points stored in global variable points[]
each point is drawn as a small square
*/
void draw_points(){

  const int R= 1;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //set color
  glColor3fv(black);

  assert(points);
  int i;
  for (i=0; i<n; i++) {
    draw_point(points[i]);
  }

}


/* recursively draw a tree rooted at the given node using preorder traversal */
void draw_node(TreeNode *node, double minX, double maxX, double minY, double maxY) {

  /* base case: leaf nodes will have left and right children which are null,
     so return here */
  if (node == NULL) return;

  Point2D p1, p2;

  draw_rect(minX, minY, maxX, maxY);

  switch(node->cuttype) {

    case VERTICAL:

    /* if the node is a vertical line, set endpoints to draw a line
      through x = node->p.x */
    p1.x = node->p.x;
    p1.y = minY;
    p2.x = node->p.x;
    p2.y = maxY;
    //printf("drawing vertical node: [%f %f] [%f %f]\n", p1.x, p1.y, p2.x, p2.y);

    draw_points();

    /* draw left and right nodes */
    draw_node(node->left, minX, node->p.x, minY, maxY);
    draw_node(node->right, node->p.x, maxX, minY, maxY);

    // now draw the line at the node
    draw_line(p1, p2);

    break;

    case HORIZONTAL:

    /* if the node is a horizontal line, set endpoints to draw a line
      through y = node->p.y */
    p1.x = minX;
    p1.y = node->p.y;
    p2.x = maxX;
    p2.y = node->p.y;

  //  printf("drawing horizontal node: [%f %f] [%f %f]\n", p1.x, p1.y, p2.x, p2.y);

    draw_points();

    /* draw bottom and top nodes */
    draw_node(node->left, minX, maxX, minY, node->p.y);
    draw_node(node->right, minX, maxX, node->p.y, maxY);

    // now draw the line at the node
    draw_line(p1, p2);

    break;

    case LEAF: /* if the node is a leaf, don't draw anything */
    break;

    default:
    printf("invalid node type\n");
    exit(1);


  };

}

/* draw the kd-tree */
void draw_kdtree() {
  assert(tree);

  /* recursively draw the tree starting at the root, using
  (0, 0) and (WINDOWSIZE, WINDOWSIZE)) as the starting bounding points */
  draw_node(tree->getRoot(), 0, WINDOWSIZE, 0, WINDOWSIZE);

}


/* ****************************** */
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); //clear the matrix

  /* the default GL window is [-1,1]x[-1,1] with the origin in the
  center the points are in the range (0,0) to (WINSIZE,WINSIZE), so
  they need to be mapped to [-1,1]x [-1,1] */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);

  // draw the points and the kd tree
  draw_points();
  draw_kdtree();

  /* execute the drawing commands */
  glFlush();
}

void init() {

  initialize_points_random();
  //print_points();

  //free current tree
  if (tree) {
    delete tree;
  }

  tree = new KdTree(points, n);

  // print the tree
  tree->kd_print();

}

/* ****************************** */
void keypress(unsigned char key, int x, int y) {

  switch(key)
  {
    case ' ':
    init();
    glutPostRedisplay();
    break;

    case 'a':
    initialize_points_grid();
    glutPostRedisplay();
    break;

    case 'b':
    initialize_points_diag();
    glutPostRedisplay();
    break;

    case 'c':
    initialize_points_circle();
    glutPostRedisplay();
    break;

    case 'd':
    initialize_points_spiral();
    glutPostRedisplay();
    break;

    case 'q':
    exit(0);
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
