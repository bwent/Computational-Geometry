## Synopsis

This program is designed for motion planning using a visibility graph.
Given a robot (ie, a source) and a final destination, this program creates
a visibility graph and then implements dijkstra's algorithm to find a path
from the source to the destination without collisions.
This program allows users to design an arena of obstacles for the robot to
navigate and then select the robot's start and end points.

## Authors

Written by Claire McCarthy and Bridget Went for Computational Geometry, Spring 2017.
Last modified 5/3/2017.

## Code
vg.cpp - contains the implementation of the algorithm to find the visibility
       graph and the path using dijkstra's
vg.h - contains the header file for vg.cpp
vis.cpp - contains the code for rendering the visibility graph, path, and arena
geom.h - header file that enables us to using geometry based structures

## Tests
A standard arena is available for the user to test the program on or the user
can design their own arena using mouse clicks.


## Compile and Run
compile: run 'make' from the command line to compile
clean: run 'make clean'

run: ./plan

NOTE: must draw counter clockwise!

test: use the following keypress features to generate test cases:
      q: quit
      c: clear the obstacles in the arena and the visibility graph
      e: indicates to the program that you are drawing a new polygon in the arena
      s: indicates to the program that you are setting the source
      f: indicates to the program that you are about to set the destination
      v: computes and draws the visibility graph
      p: implement dijkstra to find and draw the shortest path from start to end
      d: to start drawing a polygon, then press again to indicate when finished
