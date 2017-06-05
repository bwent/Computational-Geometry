/*  g1.cpp
*
*  computes the VPible region of a point p the user clicks inside a polygon P
*
*  created by bridget went and claire mccarthy 3/18/17
*  last modified 3/28/17
*
*/


#include "guard.h"

/* global variables for the guard and the original polygon */
point2D g;
vector<point2D> polygon;

/* returns 1 if p1 = p2; 0 otherwise */
int equals(point2D p1, point2D p2) {
  return (p1.x == p2.x && p1.y == p2.y);
}

/* returns 1 if line and line2 share both endpoints; 0 otherwise */
int segmentsEqual(line2D l1, line2D l2) {
  return ((equals(l1.a, l2.a) && equals(l1.b, l2.b)) ||
  (equals(l1.a, l2.b) && equals(l1.b, l2.a)));
}

/* returns the distance squared between 2 points p1 and p2*/
int dist(point2D p1, point2D p2)
{
  return (p1.x - p2.x)*(p1.x - p2.x) +
  (p1.y - p2.y)*(p1.y - p2.y);
}

/* returns the signed area defined by the segment ab with respect to c */
int signed_area2D(point2D a, point2D b, point2D c) {
  double area =  ((double)(b.x - a.x) * (double)(c.y - a.y)) -
  ((double)(c.x - a.x) * (double)(b.y - a.y));

  /* The area should be an integer. */
  if (area < -EPS) return -1;
  else if (area > EPS) return 1;
  return 0;
}

/* returns 1 if p, q, and r lie on the same line; 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  return (signed_area2D(p, q, r) == 0);
}

/* returns 1 if c is strictly to the left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) {
  return(signed_area2D(a, b, c) > 0);
}

int isReflex(int i) {

  int n = polygon.size();
  point2D polyi_minus = polygon[(i-1)%n];
  point2D polyi_plus = polygon[(i+1)%n];
  point2D polyi = polygon[i];

  return !(left(polyi_minus, polyi, polyi_plus));

}

/*---------------------------------------------------------------------
Exclusive or: TRUE iff exactly one argument is true.
---------------------------------------------------------------------*/
bool	Xor( bool x, bool y )
{
  /* The arguments are negated to ensure that they are 0/1 values. */
  /* (Idea due to Michael Baldwin.) */
  return   !x ^ !y;
}

/*---------------------------------------------------------------------
Returns true iff ab properly intersects cd: they share
a point interior to both segments.  The properness of the
intersection is ensured by using strict leftness.
---------------------------------------------------------------------*/
bool	intersect_prop( point2D a, point2D b, point2D c, point2D d )
{
  /* Eliminate improper cases. */
  if (
    collinear(a,b,c) ||
    collinear(a,b,d) ||
    collinear(c,d,a) ||
    collinear(c,d,b)
  )
  return false;

  //Zprintf("checking for proper intersection: a=(%d %d) b=(%d %d) c=(%d %d) d=(%d %d)\n", a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y);

  return
  (Xor( left(a,b,c), left(a,b,d) )
  && Xor( left(c,d,a), left(c,d,b) ));
}


/* sort by distance from the guard */
int distCmp(const point2D &a, const point2D &b) {
  return (dist(g, a) > dist(g, b));
}

/* comparison function for sorting by ccw angle */
int xyCmp(const point2D &a, const point2D &b) {
  // handle case where points are collinear
  if(a.x == b.x) {
    if (a.y == b.y) {
      return a.index > b.index;
    }
    return a.y > b.y;
  }
  return a.x > b.x;

}


/* comparison function for sorting by ccw angle */
int ccwCmp(const point2D &a, const point2D &b) {
  // handle case where points are collinear
  if (collinear(g, a, b)) {
    //  printf("COLLINEAR\n");
    return (dist(g, a) >= dist(g, b)? -1: 1);
  }
  // if p2 is to the left of the line from P to p1, it must be behind p1 in
  // sorted order
  return (signed_area2D(g,a,b) < 0 ? -1: 1);
}

/* comparison function for sorting by index angle */
int indexCmp(const point2D &a, const point2D &b) {
  // handle case where points are collinear
  if (a.index == b.index) {
    return (dist(a, polygon[a.index]) <= dist(b, polygon[b.index]));
  }
  // if p2 is to the left of the line from P to p1, it must be behind p1 in
  // sorted order
  return (a.index > b.index);
}



/* returns 1 if there is a nonproper intersection between segments l1 and l2
* from: Computational Geometry in C (O'Rourke, 1994): chapter 7
*/
int segSegIntersect(line2D l1, line2D l2, point2D &p) {
  point2D a = l1.a;
  point2D b = l1.b;
  point2D c = l2.a;
  point2D d = l2.b;

  double denom = a.x * (double)( d.y - c.y ) +
  b.x * (double)( c.y - d.y ) +
  d.x * (double)( b.y - a.y ) +
  c.x * (double)( a.y - b.y );

  if (denom == 0.0) return 0;

  double num =   a.x * (double)( d.y - c.y ) +
  c.x * (double)( a.y - d.y ) +
  d.x * (double)( c.y - a.y );
  //if ( (num == 0.0) || (num == denom) ) printf("V\n");
  double s = num / denom;

  num = -( a.x * (double)( c.y - b.y ) +
  b.x * (double)( a.y - c.y ) +
  c.x * (double)( b.y - a.y ) );
  //  if ( (num == 0.0) || (num == denom) ) printf("V\n");
  double t = num / denom;


  if((0.0 <= s) && (s < 1.0) && (0.0 <= t) && (t < 1.0)) {
    p.x = a.x + s * (b.x - a.x);
    p.y = a.y + s * (b.y - a.y);
    return 1;
  } else if ((0.0 > s) || (s > 1.0) || (0.0 > t) || (t > 1.0) ) {
    return 0;
  }

  return 0;

}

/* casts a ray from ab to the segment bc, and returns 1 if a nonproper
* intersection exists, and stores this point in p; returns 0 otherwise.
* from: Computational Geometry in C (O'Rourke, 1994): chapter 7
*/
int segRayIntersect(point2D a, point2D b, point2D c, point2D d, point2D &p) {

  double s, t;
  double num, denom;

  denom = (double)a.x * (d.y - c.y) +
  (double)b.x * (c.y - d.y) +
  (double)d.x * (b.y - a.y) +
  (double)c.x * (a.y - b.y) ;

  //printf("denom is: %f\n", denom);
  if (denom == 0.0)
  return 0;

  num = (double)a.x * (d.y - c.y) +
  (double)c.x * (a.y - d.y) +
  (double)d.x * (c.y - a.y) ;

  //printf("num is: %f\n", num);

  s = num / denom;
  //printf("s = %f", s);

  //printf("s is: %f\n", s);
  num = -((double)a.x * (c.y - b.y) +
  (double)b.x * (a.y - c.y) +
  (double)c.x * (b.y - a.y));
  t = num / denom;

  if ( (0.0 <= s) && (0.0 <= t) && (t <= 1.0) ){

    p.x = (int)(a.x + s * (b.x - a.x));
    p.y = (int)(a.y + s * (b.y - a.y));
    return 1;
  }
  return 0;

}



//function to determine if the line passed intersects with any polygon lines
//Returns 1 if the line hits an obstacle.
// Returns 2 if the line intersects at a vertex
//Return 0 if it does not.
int crossesPolygon(line2D line, vector<point2D> points) {
  line2D l;
  int n = points.size();

  //int i;
  for (int i = 0; i < n; ++i) {
    /* make a line from the next 2 consecutive points */
    l.a = points[i];
    l.b = points[(i+1)%n];

    /* check to see if the line intersects this segment */
    if (intersect_prop(line.a,line.b, l.a, l.b)) {

      //printf("SEGMENTS INTERSECT1: (%d %d)(%d %d) (%d %d)(%d %d)\n", line.a.x, line.a.y, line.b.x, line.b.y, l.a.x, l.a.y, l.b.x, l.b.y);
      return 1;
    }
  }


  return 0;
}

/* returns 1 if the polygon is simple; 0 otherwise*/
int isSimplePoly(vector<point2D> poly) {
  line2D line1, line2;
  point2D p;
  for (int i = 0; i <= poly.size()-2; i++) {
    for (int j = i + 2; j <= poly.size()-2; j++) {
      line1.a = poly[i];
      line1.b = poly[i+1];
      line2.a = poly[j];
      line2.b = poly[j+1];
      if (!segmentsEqual(line1, line2)) {
        if (intersect_prop(line1.a, line1.b, line2.a, line2.b)) {
          return 0;
        }
      }
    }
  }
  return 1;
}



/* returns 1 if point is inside the polygon; 0 otherwise*/
int isInside(point2D p, vector<point2D> poly)
{
  point2D extreme = {INF, p.y};
  line2D l1;
  l1.a = p;
  l1.b = extreme;
  line2D l2;

  point2D tmp;

  int count = 0;

  for (int i = 0; i < poly.size(); i++) {
    if (i == poly.size()-1) {
      l2.a = poly[i];
      l2.b = poly[0];
    } else {
      l2.a = poly[i];
      l2.b = poly[i+1];
    }
    /* count the number of intersections with the polygon */
    if (intersect_prop(l1.a, l1.b, l2.a, l2.b)) {
      count++;
    }
  }
  return count&1;
}





/* casts a ray from the line segment between the guard and point i on the polygon.
returns a vector of intersections with other segments of the polygon */
vector<point2D> rayCast(int i) {
  vector<point2D> result;

  if (polygon.empty())
  return result;

  int n = polygon.size();
  point2D p;

  // /* look at all other segments in the polygon*/
  for (int j = 0; j < n; ++j) {
    /* if the ray hits a boundary of the polygon, add this intersection point
    to the VPible polygon */
    if (segRayIntersect(g, polygon[i], polygon[j], polygon[(j+1)%n], p)) {
      p.index = (j+1)%n;
      printf("point's index = %d\n", p.index);
      result.push_back(p);
      // VP.push_back(p);
    }
  }

  return result;
}

int onPolygon(point2D p) {
  for (int i = 0; i < polygon.size(); ++i) {
    if (equals(p, polygon[i])) return 1;
  }
  return 0;
}


/* brute force algorithm to compute the VPible polygon starting from point P */
vector<point2D> findVisiblePoly(vector<point2D> points, point2D P) {

  vector<point2D> VP; // the VPibility polygon to return

  // save as global varss
  polygon = points;
  g = P;

  /* ensure the user drew a simple polygon */
  if (!isSimplePoly(points)) {
    printf("You did not enter a simple polygon\n");
    return VP;
  }

  /* ensure the point clicked is inside the polygon */
  if (!isInside(g, points)) {
    printf("The guard is not inside the art gallery\n");
    return VP;
  }


  /* sort the points in ccw order */
//  sort(polygon.begin(), polygon.end(), &ccwCmp);
  // printf("initial points:\n");
  // for (int i = 0; i < polygon.size(); ++i) {
  //   polygon[i].index = i;
  //   printf("polygon point: (%d %d), index = %d\n", polygon[i].x, polygon[i].y, polygon[i].index);
  // }

  int n = polygon.size();

  /* draw a line segment between the guard and every vertex of the polygon */
  line2D line;
  line.a = g;

  for (int i = 0; i < n; i++) {
    /* set the other endpoint of the line segment*/
    line.b = polygon[i];

    /* if this segment intersects the polygon, it is not visible, so move on */
    if (crossesPolygon(line,polygon)) {
      continue;
    }

    /* else, this point is visible; add it to the VPible polygon */
    polygon[i].index = i;
    VP.push_back(polygon[i]);

    if (isReflex(i)) {
      printf("point %d is reflex. casting ray...\n", i);
      /* cast a ray from the segment between the guard and this point
      to find its intersection with the polygon*/
      vector<point2D> intersections;
      intersections = rayCast(i);

      bool visible = true;
      printf("num intersections: %lu\n", intersections.size());
      for (int j = 0; j < intersections.size(); ++j) {
        visible = true;
        for (int k = 0; k < polygon.size(); ++k) {
          if (i  != j) {
            if (intersect_prop(g, intersections[j], polygon[k], polygon[(k+1)%n])) {
              visible = false;
              break;
            }
          }

        }
        if (visible) {
          VP.push_back(intersections[j]);
        }

      }


    }
  }


  /* now sort by index to ensure that the points are drawn in the correct order */
  sort(VP.begin(), VP.end(), &indexCmp);

  printf("\nsize of visible polygon: %lu\n", VP.size());

  return VP;

}
