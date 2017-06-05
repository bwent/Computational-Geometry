## Synopsis

This program is designed for motion planning using extended obstacles.
Given a robot (ie, a source) and a final destination, this program
implements and compares a search-based unifrom grid planner.

This program allows users to design an arena of obstacles for the robot to
navigate and then select the robot's start and end points.

This program uses two scenarios: a rectangular robot translating in 2D and a rectangular robot translating and rotating in 2D.
We use a generic A* search planner with a uniform grid sampling.

## Authors

Written by Claire McCarthy and Bridget Went for Computational Geometry, Spring 2017.
Last modified 5/19/2017.

## Code

The code computes the shortest path using the standard A* algorithm implemented
using an STL priority queue of states (x,y, and theta). We also maintain a map
of states to their best-cost so far as well as their predecessor state (used
to reassemble the path).

The following optimizations were added to the code to improve efficiency (both
with respect to time and space):

  - Using an occupancy grid to implement dynamic programming and cache "isfree"
    positions on the grid (WINDOWSIZE * WINDOWSIZE * ANGLES);
    when checking if a state is free, we first check if we have already calculated
    the "freeness" of this position in the grid, and if so return it. If not
    we set the occupancy grid accordingly. This greatly improved our algorithm's
    efficiency.
  - When checking for "isfree" we first check if any of the 4 endpoints of the robot
    are inside any of the polygons. Then we check the bounding box of the robot
    against the bounding box of all the polygons in the arena. This also improved
    efficiency.
  - When looking at angles of rotation, we only check theta +/- 1, i.e. the angles
    directly before and after the current angle. It can be shown that this covers
    all possible thetas, since we will continue to generate new possible states
    as long as the theta is between 0 and 360. Moreover, if the current state is
    free without rotating, we simply set it to free and keep looking rather than
    checking neighboring rotation states.


## Tests
A standard arena is available for the user to test the program on or the user
can design their own arena using mouse clicks.



## Compile and Run
compile: run 'make' from the command line to compile
clean: run 'make clean'

run:

test: use the following keypress features to generate test cases:
      i: initialize the default arena
      d: start/finish drawing a polygon
      s: select start point for the robot
      f: select finish point for the robot
      p: run a* to compute shortest path
      q: quit
