/*
   Laura Toma
*/


#ifndef __geom_h
#define __geom_h

#define INF 10000
#define EPS 1e-3

typedef struct _point2d {
  int x,y;
  int poly_index; // obstacle to which this belonds (-1 for source, -2 for dest)
  int index_in_poly; // the index of the point in the polygon
} point2D;


typedef struct _line2D {
    point2D a, b;
} line2D;


typedef struct _rect2D  {
    point2D origin;
    float width, height;
} rect2D;

typedef struct _robot2D  {
    double x, y;
    point2D vertex_bl, vertex_tl, vertex3_tr, vertex_br;
    float width, height;
} robot2D;

#endif
