/*
   Laura Toma
*/


#ifndef __geom_h
#define __geom_h

#define INF 10000
#define EPS 1e-15

typedef struct _point2d {
  int x,y;
  int index; /* the index of the left-endpoint of the segment which this point
                lies on in the polygon (might be the index of hte point itself) */
} point2D;


typedef struct _line2D {
    point2D a, b;
} line2D;


typedef struct _rect2D  {
    point2D origin;
    float width, height;
} rect2D;





#endif
