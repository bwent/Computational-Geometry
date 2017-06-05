#include "geom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;


#ifndef __guard_h
#define __guard_h



vector<point2D> findVisiblePoly(vector<point2D> points, point2D P);

vector<point2D> rayCast(int i);

//int segSegIntersect(point2D a, point2D b, point2D c, point2D d,point2D &p);
int segRayIntersect(point2D a, point2D b, point2D c, point2D d, point2D &p);
int crossesPolygon(line2D line, vector<point2D> points);
int isSimplePoly(vector<point2D> poly);
int isInside(point2D p, vector<point2D> poly);

/* sorting comparison functions */
int distCmp(const point2D &a, const point2D &b);
int ccwCmp(const point2D &a, const point2D &b);
int indexCmp(const point2D &a, const point2D &b);

//int isReflex(vector<point2D> points, int i);

int equals(point2D p1, point2D p2);
int segmentsEqual(line2D l1, line2D l2);
int dist(point2D p1, point2D p2);
int signed_area2D(point2D a, point2D b, point2D c);
int collinear(point2D p, point2D q, point2D r);
int left (point2D a, point2D b, point2D c);
int between(point2D a, point2D b, point2D c);
int onPolygon(point2D p);

#endif
