/*
   Laura Toma
*/


#ifndef __geom_h
#define __geom_h

#define INF 1e15
#define EPS 1e-3
#define ANGLES 360

const int ROBOT_WIDTH = 40;
const int ROBOT_HEIGHT = 5;


typedef struct _point2d {
  int x,y;
} point2D;

typedef struct _line2D {
    point2D a, b;
} line2D;


typedef struct _rect2D  {
    point2D origin;
    float width, height;
} rect2D;


typedef struct _robot2D  {
    point2D p0, p1, p2, p3;
    int theta;
} robot2D;

#endif
