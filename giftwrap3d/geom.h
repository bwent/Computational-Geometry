#ifndef __geom_h
#define __geom_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
//#include <queue>
#include <unordered_set>


#define BIG_INT 9e5

using namespace std;

struct point3d {
  int x,y,z;
} ;

struct face3d {
  point3d *p1, *p2, *p3;  //the vertices on this face (in ccw as looking from outside)
  face3d *f12; //face to the left of p1p2
  face3d *f23; //face to the left of p2p3
  face3d *f31; //face to the left of p3p1
} ;

struct edge3d {
  point3d *p, *q;
  face3d *ccw_face;

};

int signed_volume(point3d a, point3d b, point3d c, point3d d);

/* return 1 if the two points are equal; 0 otherwise */
int isEqual(point3d a, point3d b);

/* return 1 if p,q,r, t on same plane, and 0 otherwise */
int coplanar(point3d p, point3d q, point3d r, point3d t);

/* return 1 if d is  strictly left of abc; 0 otherwise */
int left(point3d a, point3d b, point3d c, point3d d);

struct edge_equals {
public:
  bool operator()(const edge3d &e1, const edge3d &e2) const {
    if (isEqual(*e1.p, *e2.p) && isEqual(*e1.q, *e2.q)) {
      return true;
    }
    if (isEqual(*e1.p, *e2.q) && isEqual(*e1.q, *e2.p)) {
      return true;
    }
    return false;
  }
};

struct edge_hash {
public:
  size_t operator()(const edge3d &e) const {


    unsigned char* k = reinterpret_cast<unsigned char*>(e.p);
    unsigned char* l = reinterpret_cast<unsigned char*>(e.q);

    size_t h = 2166136261;

    for (unsigned int i = 0; i < 2 * sizeof(k); ++i)
        h = (h * 16777619) ^ (k[i] + l[i]);

    return h;
  }
};



point3d* findPointWithMinAngle(edge3d &e, vector<point3d> &points);

int edgeProcessed(edge3d &e);

//edge3d makeEdge(point3d p, point3d q);

face3d findInitialFace(vector<point3d> &points);


/* compute and return the convex hull of the points */
vector<face3d> gift_wrapping_hull(vector<point3d> &points);




#endif
