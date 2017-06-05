#include "geom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;


#ifndef __vg_h
#define __vg_h


/* a node in the visibility graph */
struct graph_node {
  point2D point;
  int id; // index in the visibility graph

  double dist; // current min distance from source
  point2D prev; //previous node that yields this min distance

  vector<graph_node*> adjacencies; // store pointers to graph nodes in visibility graph
};

/* comparison function for updating nodes in the priority queue */
struct compare_pairs{
    public:
        bool const operator()(const pair<double,graph_node> &n1, const pair<double,graph_node> &n2) {
            return (n1.first > n2.first) ;
        }
};

int equals(point2D a, point2D b);
int segmentsEqual(line2D l1, line2D l2);
int get_dist(point2D p1, point2D p2);

int segSegIntersect(line2D l1, line2D l2);
int hits_obstacle(line2D line, vector<line2D> segments);

vector<line2D> find_obstacles(vector<vector<point2D> > obstacles);
void add_endpoints(point2D start, point2D end);

graph_node find_node(point2D point);
vector<line2D> add_poly_edges(vector<line2D> segments, vector<vector<point2D> > obstacles);

vector<line2D> compute_visibility_graph(point2D start, point2D end, vector<vector<point2D> > obstacles);

vector<line2D> dijkstra(point2D start, point2D end);

vector<line2D> generic_planner(point2D source, point2D dest, vector<vector<point2D> > obstacles);
int isFree(point2D point, robot2D robot);
#endif
