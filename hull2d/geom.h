#ifndef __geom_h
#define __geom_h


typedef struct _point2d {
  int x,y;
} point2D;


//a list of points
typedef struct _pointNode pointNode;
struct _pointNode  {
  point2D p;
  pointNode* next;
};

//int distSq(point2D p1, point2D p2);

int orientation(point2D p, point2D q, point2D r);

point2D *P; // point with the minimum Y value for comparison

/* returns 2 times the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c);


/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);


/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c);

void swap (point2D *a, point2D *b);
/* compute the convex hull of the points in p; the points on the CH
   are returned as a list.
*/
pointNode* graham_scan(point2D* p, int n);

int compare(const void *a, const void *q);

// stack impelmeentation functions
pointNode* push(pointNode *head,point2D a);
pointNode* pop(pointNode *head);

void printer(point2D *p, int n);

int signed_area(point2D a, point2D b, point2D c);

#endif
