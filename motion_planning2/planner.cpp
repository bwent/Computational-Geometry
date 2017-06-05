/*  vg.cpp
*
*  computes the visibility graph
*
*  created by bridget went and claire mccarthy 4/25/17
*
*/


#include "planner.h"


#define PRINT if(0)

int*** occ_grid;

int num_obstacles = 0;
int num_vertices = 0;

const int NOT_SET = -99999;
const int FREE = 0;
const int NOT_FREE = 1;

int robotHeight = 30;
int robotWidth = 20;
point2D start_pos;
point2D end_pos;
vector<vector<point2D> > polygons;

map<state, pair<state,double>, compare_keys> state_map;

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
Returns TRUE if point c lies on the closed segement ab.
First checks that c is collinear with(s.theta)and b.
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
  /* The arguments are negated to ensure that they are 0/1 values-> */
  /* (Idea due to Michael Baldwin.) */
  return   !x ^ !y;
}

/*---------------------------------------------------------------------
Returns true iff ab properly intersects cd: they share
a point interior to both segments.  The properness of the
intersection is ensured by using strict leftness->
---------------------------------------------------------------------*/
bool	intersect_prop( point2D a, point2D b, point2D c, point2D d )
{
  /* Eliminate improper cases-> */
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
Returns TRUE if segments ab and cd intersect, properly or improperly.
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

  double  s, t;       /* The two parameters of the parametric eqns-> */
  double num, denom;  /* Numerator and denoninator of equations-> */
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
      /* add the next segment to the list of segments */
      line2D seg = {.a = obstacles[i][j], .b = obstacles[i][(j+1)%n]};

      segments.push_back(seg);

    }
  }

  return segments;
}

// int isInside(point2D p, vector<point2D> poly)
// {
//   point2D extreme = {INF, p.y};
//   line2D l1;
//   l1.a = p;
//   l1.b = extreme;
//   line2D l2;
//
//   int count = 0;
//
//   for (int i = 0; i < poly.size(); i++) {
//     if (i == poly.size()-1) {
//       l2.a = poly[i];
//       l2.b = poly[0];
//     } else {
//       l2.a = poly[i];
//       l2.b = poly[i+1];
//     }
//     /* count the number of intersections with the polygon */
//     if (segSegIntersect(l1, l2)) {
//       count++;
//     }
//   }
//   return count&1;
// }

int isInside(point2D p, vector<point2D> poly)
{
  int i, j, c = 0;
  for (i = 0, j = poly.size()-1; i < poly.size(); j = i++) {
    if ( ((poly[i].y>p.y) != (poly[j].y>p.y)) &&
    (p.x < (poly[j].x-poly[i].x) * (p.y-poly[i].y) / (poly[j].y-poly[i].y) + poly[i].x) )
    c = !c;
  }
  return c;
}
/* only works on convex simple polygons drawn counterclockwise....*/
// int isInside(point2D p, vector<point2D> poly) {
//   line2D edge;
//   for (int i = 0; i < poly.size(); i++) {
//
//     if (i == poly.size()-1) {
//       edge.a = poly[i];
//       edge.b = poly[0];
//     } else {
//       edge.a = poly[i];
//       edge.b = poly[i+1];
//     }
//
//     if (!left(edge.a, edge.b, p)) {
//       return 0;
//     }
//
//   }
//   return 1;
// }

bool robot_in_bounds(robot2D robot) {

  if (robot.p0.x < 0 || robot.p0.x >= 500) {
    return false;
  }
  if (robot.p0.y < 0 || robot.p0.y >= 500) {
    return false;
  }
  if (robot.p1.x < 0 || robot.p1.x >= 500) {
    return false;
  }
  if (robot.p1.y < 0 || robot.p1.y >= 500) {
    return false;
  }
  if (robot.p2.x < 0 || robot.p2.x >= 500) {
    return false;
  }
  if (robot.p2.y < 0 || robot.p2.y >= 500) {
    return false;
  }
  if (robot.p3.x < 0 || robot.p3.x >= 500) {
    return false;
  }
  if (robot.p3.y < 0 || robot.p3.y >= 500) {
    return false;
  }

  return true;
}


bool is_free(robot2D robot) {

  vector<point2D> poly;
  line2D edge1, edge2, edge3, edge4, polyEdge;

  if(!robot_in_bounds(robot))  {
    return false;
  }

  edge1.a = robot.p0;
  edge1.b = robot.p1;
  edge2.a = robot.p1;
  edge2.b = robot.p2;
  edge3.a = robot.p2;
  edge3.b = robot.p3;
  edge4.a = robot.p3;
  edge4.b = robot.p0;


  for (int i = 0; i < polygons.size(); i++) {
    poly = polygons[i];
    if (isInside(robot.p0, poly) || isInside(robot.p1, poly) ||
    isInside(robot.p2, poly) || isInside(robot.p3, poly)) {
      return false;
    }
    for (int j = 0; j < poly.size(); j++) {
      if (j == poly.size()-1) {
        polyEdge.a = poly[j];
        polyEdge.b = poly[0];
      } else {
        polyEdge.a = poly[j];
        polyEdge.b = poly[j+1];
      }

      if (segSegIntersect(edge1, polyEdge)) {
        return false;
      } else if (segSegIntersect(edge2, polyEdge)) {
        return false;
      } else if (segSegIntersect(edge3, polyEdge)) {
        return false;
      } else if (segSegIntersect(edge4, polyEdge)) {
        return false;
      }
    }
  }
  return true;
}

void printPolygon(vector<point2D> poly) {
  for (int i = 0; i < poly.size(); i++) {
    PRINT printf("Poly vertex %d at (%d, %d)\n", i, poly[i].x, poly[i].y);
  }

}

bool is_goal(state s) {
  return (s.x == end_pos.x && s.y == end_pos.y);
}

bool at_start(state s) {
  return (s.x == start_pos.x && s.y == start_pos.y);
}


/* returns a vector containing all successor state_map of a state s, i.e.
the state_map which the robot can move to from state s*/
vector<state> find_neighbors(state s) {

  vector<state> v;

  state next_state;
  robot2D robot;
  int x,y,theta;

  /* check all directions around the current state */
  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      if (i == j || i+j == 0) {
        continue;
      }

      /* update the next state*/
      next_state.x = s.x+i;
      next_state.y = s.y+j;
      next_state.theta = s.theta;
      robot = move_robot(next_state);

      x = next_state.x;
      y = next_state.y;
      theta = next_state.theta;

      // make sure we don't run off the boundary of the window
      if (x < 0 || y < 0 || x >= WINDOWSIZE || y >= WINDOWSIZE) {
        break;
      }

      if (occ_grid[x][y][theta] == FREE) {
        PRINT printf("retrieve (%d,%d,%d) FREE\n", x,y,theta);
        v.push_back(next_state);
        continue;
      } else {
        // NOT SET
        if (is_free(move_robot(next_state))) {
          PRINT printf("(%d,%d,%d) IS FREE\n", x,y,theta);
          occ_grid[x][y][theta] = FREE;
          v.push_back(next_state);
          continue;
        }
      }
      // if the robot is not free without accounting for rotations, check angles
      for (int k = -1; k <= 1; ++k) {
        if (k == 0 || s.theta + k < 0 || s.theta + k > ANGLES) {
          continue;
        }
        //check the angles before and after the current theta
        next_state.theta =theta = s.theta+k;
        robot = move_robot(next_state);

        /* check if this position is FREE and add it to the list of neighbors */
        if (occ_grid[x][y][theta] == NOT_FREE) {
          PRINT printf("retrieve (%d,%d,%d) NOT FREE\n", x,y,theta);
          continue;
        } else if (occ_grid[x][y][theta] == FREE) {
          PRINT printf("retrieve (%d,%d,%d) FREE\n", x,y,theta);
          v.push_back(next_state);

        } else {
          // NOT SET
          if (is_free(move_robot(next_state))) {
            PRINT printf("(%d,%d,%d) IS FREE\n", x,y,theta);
            occ_grid[x][y][theta] = FREE;
            v.push_back(next_state);

          } else {
            PRINT printf("(%d,%d,%d) IS NOT FREE\n", x,y,theta);
            occ_grid[x][y][theta] = NOT_FREE;
          }
        }

      }// for k
  } // for j
} // for i

return v;

}

/* A* heuristic: returns the euclidean distance from a state s to the goal state */
double euclidean_dist(state s) {
  point2D p = {.x = s.x, .y = s.y};
  return (get_dist(p, end_pos));
}

/* returns the cost of moving from s1 to s2 (1 if s1 is directly n,s,e,w of s2, root 2
if it's diagonal) */
double cost_of_edge(state s1, state s2) {
  point2D p1 = {.x = s1.x, .y = s1.y};
  point2D p2 = {.x = s2.x, .y = s2.y};
  return (get_dist(p1, p2));
  //return 1;
}


/* creates and returns a new state computed by moving robot to position x,y
and rotating by angle */
robot2D move_robot(state s) {

  robot2D moved;

  int len = ROBOT_HEIGHT;
  int width = ROBOT_WIDTH;

  point2D a = {0,0};
  point2D b = {width,0};
  point2D c = {width,len};
  point2D d = {0,len};

  moved.p0.x = a.x * cos(s.theta) - a.y * sin(s.theta);
  moved.p0.y = a.x * sin(s.theta) + a.y * cos(s.theta);
  moved.p1.x = b.x * cos(s.theta) - b.y * sin(s.theta);
  moved.p1.y = b.x * sin(s.theta) + b.y * cos(s.theta);
  moved.p2.x = c.x * cos(s.theta) - c.y * sin(s.theta);
  moved.p2.y = c.x * sin(s.theta) + c.y * cos(s.theta);
  moved.p3.x = d.x * cos(s.theta) - d.y * sin(s.theta);
  moved.p3.y = d.x * sin(s.theta) + d.y * cos(s.theta);

  moved.p0.x += s.x;
  moved.p0.y += s.y;
  moved.p1.x += s.x;
  moved.p1.y += s.y;
  moved.p2.x += s.x;
  moved.p2.y += s.y;
  moved.p3.x += s.x;
  moved.p3.y += s.y;


  return moved;

}

/* initialize the occupancy grid used for caching:
    - NOT_SET means we haven't come across the state before
    - FREE = in the free space of the robot
    - NOT_FREE = not in the free space of the robot (i.e. cannot move here) */
void init_occupancy_grid() {

  occ_grid = new int**[WINDOWSIZE];

  for (int i = 0; i <= WINDOWSIZE; i++) {

    occ_grid[i] = new int*[WINDOWSIZE];

    for (int j = 0; j <= WINDOWSIZE; j++) {

      occ_grid[i][j] = new int[ANGLES];

      for (int k = 0; k <= ANGLES; ++k) {
        occ_grid[i][j][k] = NOT_SET;
      }
    }
  }

}

void print_state(state s) {
  PRINT printf("--------------------------------------------------\n");
  PRINT printf("state: robot is at (%d,%d), angle = %d, cost = %f \n",
  s.x, s.y, s.theta, state_map.find(s)->second.second);
  if (state_map.find(s) != state_map.end()) {
    PRINT printf("previous state: robot is at (%d,%d), angle = %d, cost = %f\n",
    state_map[s].first.x, state_map[s].first.y, state_map[s].first.theta, state_map[s].second);
  } else {
    PRINT printf("no predecessor state.\n");
  }

  PRINT printf("--------------------------------------------------\n\n");

}

double priority(state s1, state s2) {

  return (state_map[s1].second + cost_of_edge(s1, s2) + euclidean_dist(s2));
}


vector<state> a_star(point2D source, point2D dest, vector<vector<point2D> > obstacles) {

  vector<state> path; // the path to return
  polygons = obstacles;
  init_occupancy_grid();

  start_pos = source;
  end_pos = dest;

  map<state,pair<state,double> >::iterator it = state_map.begin();
  while (it != state_map.end()) {

  }

  assert(path.empty());

  priority_queue<pair<double,state>, vector<pair<double, state> >, compare_states > PQ; // the priority queue

  /* add the start position to the prioroity queue*/
  state init = {.x = source.x, .y = source.y, .theta = 0};

  if (!is_free(move_robot(init))) {
    PRINT printf("initial state is not free.");
    exit(1);
  }

  /* add the initial robot to the PQ */
  PQ.push(make_pair(0.0, init));
  //state_map.insert(make_pair(init, make_pair(null, 0.0)));

  state end_state;

  int states_explored = 0;

  while (!PQ.empty()) {
    /* while the queue is not empty, look at the next node */
    state s = PQ.top().second;
    PQ.pop();
    states_explored++;

    /* if we're at the destination, we're done. */
    if (is_goal(s)) {
      PRINT printf("reached destination!\n");
      end_state = s;
      break;
    }

    vector<state> neighbors = find_neighbors(s);

    /* loop over the neighbors of s */
    for(int i = 0; i < neighbors.size(); ++i) {

      state s2 = neighbors.at(i);

      /* if it's the first time we've seen this state, add it to the PQ
        with prioirty INFINITY */
      if (state_map.find(s2) == state_map.end()) {
        state_map.insert(make_pair(s2, make_pair(s,INF)));
      }

      /* if we found a better path through this state, update the state
          map to this priority, and set predecessor node to s */
      if (priority(s,s2) < state_map[s2].second) {

        state_map[s2].first = s;
        state_map[s2].second = priority(s,s2);

        /* now push onto the priority queue with the updated priority */
        PQ.push(make_pair(priority(s,s2), s2));

      }

    }
  }

  /* reassemble the path */
  line2D l;
  state curr_state = end_state;

  PRINT printf("\n\nreassembling path...\n\n");
  PRINT printf("pred: (%d,%d)\n", state_map[curr_state].first.x,state_map[curr_state].first.y);

  PRINT printf("Size of map: %lu\n", state_map.size());

  while (!at_start(curr_state)) {
    state s = curr_state;
    state pred = state_map[s].first;

    print_state(curr_state);

    /* push onto the path, given by a vector of states */
    path.push_back(s);
    curr_state = state_map[s].first;

    print_state(curr_state);

  }

  return path; // return the shortest path found
}
