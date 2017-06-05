#include "geom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <set>

using namespace std;
const int WINDOWSIZE = 500;

#ifndef __vg_h
#define __vg_h


struct state {
  int x, y, theta;

};

/* comparison function for updating nodes in the priority queue */
struct compare_states{
    public:
        bool operator()(const pair<double,state>& s1, const pair<double,state>& s2) const {
            return (s1.first > s2.first) ;
        }
};

struct compare_keys {
public:
  bool operator()(const state& s1, const state& s2)const  {
    if (s1.x == s2.x) {
      if (s1.y == s2.y) {
        return s1.theta > s2.theta;
      }
      return s1.y > s2.y;
    }
    return s1.x > s2.x;
  }

};

int signed_area2D(point2D a, point2D b, point2D c);
int collinear(point2D p, point2D q, point2D r);
int left (point2D a, point2D b, point2D c);
int left_on(point2D a, point2D b, point2D c);
int equals(point2D p1, point2D p2);
int segmentsEqual(line2D l1, line2D l2);
int get_dist(point2D p1, point2D p2);
bool	between( point2D a, point2D b, point2D c );
bool	Xor( bool x, bool y );
bool	intersect_prop( point2D a, point2D b, point2D c, point2D d );
bool	intersect( point2D a, point2D b, point2D c, point2D d );
int	segSegIntersect( line2D l1, line2D l2 );
int hits_obstacle(line2D line, vector<line2D> segments);
vector<line2D> find_obstacles(vector<vector<point2D> > obstacles);
int isInside(point2D p, vector<point2D> poly);
bool robot_in_bounds(robot2D robot);
bool is_free(robot2D robot);
void printPolygon(vector<point2D> poly);
bool is_goal(state s);
bool at_start(state s);
vector<state> find_successors(state s);
double euclidean_dist(state s);
double cost_of_edge(state s1, state s2);
robot2D move_robot(state s);
void print_state(state s);
double priority(state s1, state s2);
vector<state> a_star(point2D source, point2D dest, vector<vector<point2D> > obstacles);

//
#endif
