/*  vg.cpp
*
*  computes the visibility graph
*
*  created by bridget went and claire mccarthy 4/25/17
*
*/


#include "vg.h"

vector<graph_node> vis_graph;
vector<line2D> adj_list;
vector<vector<point2D> > polygons;

int num_obstacles = 0;
int num_vertices = 0;

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

int left_on(point2D a, point2D b, point2D c) {
  return(signed_area2D(a, b, c) >= 0);
}

/* returns 1 if p1 = p2; 0 otherwise */
int equals(point2D p1, point2D p2) {
  return (p1.x == p2.x && p1.y == p2.y);
}

/* returns 1 if line and line2 share both endpoints; 0 otherwise */
int segmentsEqual(line2D l1, line2D l2) {
  return ((equals(l1.a, l2.a) && equals(l1.b, l2.b)) ||
  (equals(l1.a, l2.b) && equals(l1.b, l2.a)));
}

/* returns the straight line distance between 2 points p1 and p2*/
int get_dist(point2D p1, point2D p2) {
  return sqrt((p1.x - p2.x)*(p1.x - p2.x) +
  (p1.y - p2.y)*(p1.y - p2.y));
}

/*---------------------------------------------------------------------
Returns TRUE iff point c lies on the closed segement ab.
First checks that c is collinear with a and b.
---------------------------------------------------------------------*/
bool	between( point2D a, point2D b, point2D c )
{

  if ( !collinear( a, b, c ) )  return  false;

  /* If ab not vertical, check betweenness on x; else on y. */
  if ( a.x != b.x )
  return ((a.x <= c.x) && (c.x <= b.x)) ||
  ((a.x >= c.x) && (c.x >= b.x));
  else
  return ((a.y <= c.y) && (c.y <= b.y)) ||
  ((a.y >= c.y) && (c.y >= b.y));
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

   return
         Xor( left(a,b,c), left(a,b,d) )
      && Xor( left(c,d,a), left(c,d,b) );
}

/*---------------------------------------------------------------------
Returns TRUE iff segments ab and cd intersect, properly or improperly.
---------------------------------------------------------------------*/
bool	intersect( point2D a, point2D b, point2D c, point2D d )
{
  if      ( intersect_prop( a, b, c, d ) )
  return  true;
  else if (   between( a, b, c )
  || between( a, b, d )
  || between( c, d, a )
  || between( c, d, b )
)
return  true;
else    return  false;
}

/*---------------------------------------------------------------------
Returns TRUE iff (a,b) is a proper internal *or* external
diagonal of P, *ignoring edges incident to a and b*.
---------------------------------------------------------------------*/
int diagonalie(graph_node a, graph_node b)
{

  // if (a.point.poly_index != b.point.poly_index) {
  //   return false;
  // }
  /* For each edge (c,c1) of P */
  int index = a.point.poly_index;

  vector<point2D> currPoly = polygons[index];

  int n = currPoly.size();

  for (int i = 0; i < n; ++i) {
    if (equals(currPoly[i], a.point) || equals(currPoly[i], b.point)
        || equals(currPoly[(i+1)%n], a.point) || equals(currPoly[(i+1)%n], b.point)) {
          continue;
    }
    if (intersect(a.point,b.point,currPoly[i],currPoly[(i+1)%n])) {
      return false;
    }

  }
  //printf("polygon %d index %d to index %d is a diagonal\n", index, a.point.index_in_poly, b.point.index_in_poly);
  return true;
}

/*---------------------------------------------------------------------
Returns TRUE iff the diagonal (a,b) is strictly internal to the
polygon in the neighborhood of the a endpoint.
---------------------------------------------------------------------*/
int   in_cone( graph_node a, graph_node b)
{

  if (a.point.poly_index != b.point.poly_index) {
    return false;
  }
  /* For each edge (c,c1) of P */
  int index = a.point.poly_index;

  vector<point2D> currPoly = polygons[index];
  int n = currPoly.size();

   point2D prev,next;
   prev = currPoly[(a.point.index_in_poly - 1)%n];
   next = currPoly[(a.point.index_in_poly + 1)%n];

   /* If a is a convex vertex ... */
   if( left_on( a.point, next, prev ) )
       return    (left(a.point, b.point, prev )
              && left( b.point, a.point, next ));

   /* Else a is reflex: */
       return !( left_on( a.point, b.point, next )
                 && left_on( b.point, a.point, prev) );
}


/* returns 1 if there is a nonproper intersection between segments l1 and l2
* from: Computational Geometry in C (O'Rourke, 1994): chapter 7
*/
int	segSegIntersect( line2D l1, line2D l2 )
{
  point2D a, b, c, d;
  a = l1.a;
  b = l1.b;
  c = l2.a;
  d = l2.b;

  double  s, t;       /* The two parameters of the parametric eqns. */
  double num, denom;  /* Numerator and denoninator of equations. */
  int code = -1;    /* Return char characterizing intersection. */

  denom = a.x * (double)( d.y - c.y ) +
  b.x * (double)( c.y - d.y ) +
  d.x * (double)( b.y - a.y ) +
  c.x * (double)( a.y - b.y );

  /* If denom is zero, then segments are parallel: handle separately. */
  if (denom == 0.0) code = 0;
  //return  ParallelInt(a, b, c, d, p);

  num =    a.x * (double)( d.y - c.y ) +
  c.x * (double)( a.y - d.y ) +
  d.x * (double)( c.y - a.y );
  if ( (num == 0.0) || (num == denom) ) code = 0;
  s = num / denom;
  //  printf("num=%lf, denom=%lf, s=%lf\n", num, denom, s);

  num = -( a.x * (double)( c.y - b.y ) +
  b.x * (double)( a.y - c.y ) +
  c.x * (double)( b.y - a.y ) );
  if ( (num == 0.0) || (num == denom) ) code = 0;
  t = num / denom;
  //  printf("num=%lf, denom=%lf, t=%lf\n", num, denom, t);

  if ( (0.0 < s) && (s < 1.0) &&
  (0.0 < t) && (t < 1.0) )
  code = 1;
  else if ( (0.0 > s) || (s > 1.0) ||
  (0.0 > t) || (t > 1.0) )
  code = 0;

  //  p[X] = a.x + s * ( b.x - a.x );
  //  p[Y] = a.y + s * ( b.y - a.y );

  return code;
}

/* returns 1 if the line intersects one of the segments defined by the obstacles
in the arena; 0 otherwise; -1 if there are no obstacles to intersect  */
int hits_obstacle(line2D line, vector<line2D> segments) {

  int n = segments.size();

  for (int i = 0; i < n; ++i) {
    /* check to see if the line intersects the next segment */
    if (segSegIntersect(line, segments[i])) {
      return 1;
    }
  }

  return 0;
}


/* returns a vector of line segments representing the obstacles
defined by the 2D vector of polygons in the arena */
vector<line2D> find_obstacles(vector<vector<point2D> > obstacles) {

  vector<line2D> segments;

  for (int i = 0; i < num_obstacles; ++i) {
    int n = obstacles[i].size();

    for (int j = 0; j < n; ++j) {

      /* create and add a node to the visibliity graph */
      graph_node node;
      node.point = obstacles[i][j];
      node.point.poly_index = i;
      node.point.index_in_poly = j;
      vector<graph_node*> adj;
      node.adjacencies = adj;
      node.id = num_vertices;
      num_vertices++;
      vis_graph.push_back(node);

      /* add the next segment to the list of segments */
      line2D seg = {.a = obstacles[i][j], .b = obstacles[i][(j+1)%n]};

      segments.push_back(seg);

    }
  }

  return segments;
}

/* creates nodes for the endpoints and adds them to the visibility graph */
void add_endpoints(point2D start, point2D end) {
  /* add the source to the graph */
  graph_node source;
  source.dist = 0;
  source.point = start;
  source.point.poly_index = -1;

  vector<graph_node*> adj1;
  source.adjacencies = adj1;
  source.id = num_vertices;
  num_vertices++;
  vis_graph.push_back(source);

  /* add the dest to the graph */
  graph_node dest;
  dest.dist = INF;
  dest.point = end;
  dest.point.poly_index = -2;

  vector<graph_node*> adj2;
  dest.adjacencies = adj2;
  dest.id = num_vertices;
  num_vertices++;
  vis_graph.push_back(dest);
}

/* returns 1 if point is inside the polygon; 0 otherwise*/
int isInside(point2D p, vector<point2D> poly)
{
  point2D extreme = {INF, p.y};
  line2D l1;
  l1.a = p;
  l1.b = extreme;
  line2D l2;

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
    if (segSegIntersect(l1, l2)) {
      count++;
    }
  }
  return count&1;
}



/* finds and returns the visibility graph node specified by the given vertex */
graph_node find_node(point2D point) {

  graph_node found;
  for (int i = 0; i < vis_graph.size(); ++i) {
    if (equals(vis_graph.at(i).point,point)) {
      found = vis_graph.at(i);
      break;
    }
  }
  return found;
}


/* computes the visiblity graph starting from the start point with
vertices of all obstacles in the arena */
vector<line2D> compute_visibility_graph(point2D start, point2D end, vector<vector<point2D> > obstacles) {

  for (int i = 0; i < vis_graph.size(); ++i) {
    vis_graph[i].adjacencies.clear();
  }
  vis_graph.clear();
  adj_list.clear();
  polygons.clear();
  polygons = obstacles;
  num_vertices = 0;

  num_obstacles = obstacles.size();

  add_endpoints(start, end);

  vector<line2D> segments = find_obstacles(obstacles);

  /* now look at all vertices and check for visibility from all other vertices */
  for (int i = 0; i < num_vertices; ++i) {
    for (int j = 0; j < num_vertices; ++j) {

      /* skip the node itself, and any nodes that are part of the same obstacle */
      if (i == j)  {
        continue;
      }

      graph_node g1 = vis_graph.at(i);
      graph_node g2 = vis_graph.at(j);

      /* draw a line between this pair of vertices */
      line2D test = {.a = g1.point, .b = g2.point};

      if(g1.point.poly_index == g2.point.poly_index) {
        if (hits_obstacle(test, segments)) continue;
        if (diagonalie(g1, g2) && !in_cone(g1, g2)) {
          //printf("diagonal, but not in the cone: polygon %d index %d to %d\n", g1.point.poly_index, g1.point.index_in_poly, g2.point.index_in_poly);
          vis_graph.at(i).adjacencies.push_back(&vis_graph.at(j));
          adj_list.push_back(test);
        }
      } else if (!hits_obstacle(test, segments)) { /* if this line doesn't intersect an obstacle, pj is visible from pi */
        vis_graph.at(i).adjacencies.push_back(&vis_graph.at(j));
        adj_list.push_back(test);
      }
    } // for j

    //printf("vis graph at %d has %lu adjacencies\n", i, vis_graph.at(i).adjacencies.size());

  } // for i


  segments.clear();
  return adj_list;
}

/* implements dijkstra's algorithm to find the shortest path between start and
end, avoiding all obstacles */
vector<line2D> dijkstra(point2D source, point2D dest) {

  vector<line2D> path; // the path to return
  assert(path.empty());

  int n = vis_graph.size();

  /* initialize all nodes to have a min distance of INFINITY */
  for (int i = 0; i < n; ++i) {
    vis_graph[i].dist = INF;
  }

  priority_queue<pair<double, graph_node>, vector<pair<double, graph_node> >,
  compare_pairs > PQ; // the priority queue

  /* find the node containing the source point in the visibility graph */
  vis_graph[0].dist = 0;
  graph_node source_node = vis_graph[0];

  /* add this node with distance 0 to the prioroity queue*/
  PQ.push(make_pair(0, vis_graph[0]));

  while (!PQ.empty()) {
    /* while the queue is not empty, look at the next node */
    graph_node u = PQ.top().second;
    PQ.pop();

    /* if we're at the destination, we're done! */
    if (equals(u.point, dest)) {
      //      printf("reached destination!\n");
      break;
    }

    //    printf("node u has %lu adjacent vertices\n", u.adjacencies.size());

    /* look at all the vertices adjacent to this node */
    for (int i = 0; i < u.adjacencies.size(); ++i) {

      graph_node *v = u.adjacencies[i];
      double weight = get_dist(u.point, v->point);

      //  printf("u dist = %f, v dist = %f\n", u.dist, v->dist);

      /* if there is a shorter path to v through u, update min distance  */
      if (v->dist > u.dist + weight) {

        v->dist = u.dist + weight;
        v->prev = u.point;

        PQ.push(make_pair(v->dist, *v)); /* add this node to the priority queue
        (weird C++ thing that the PQ doesn't automatically heapify on its own...)*/
      }
    }
  }

  /* reconstruct the shortest path by tracing to previous nodes in the path
  starting at the dest node, until we reach the source node */
  line2D l;
  graph_node p = find_node(dest);

  while (!equals(p.point, source)) {
    l.a = p.point;
    l.b = p.prev;
    p = find_node(p.prev);
    path.push_back(l);
  }


  return path; // return the shortest path found

}
