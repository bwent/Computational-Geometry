## Synopsis

This program is designed to find the visible area of a guard (point) inside an art gallery (polygon).
This program implements an algorithm that computes and displays the part of the art gallery visible to the guard.
Additionally, this program allows the user to animate and move the security guard and re-compute the visibility polygon.

## Authors

Written by Claire McCarthy and Bridget Went for Computational Geometry, Spring 2017.
Last modified 4/25/2017.

## Code
guard.cpp - contains the implementation of the algorithm to find the visibility polygon
guard.h - contains the header files for guard.cpp
vis.cpp - contains the code for rendering the visibility polygon and implementing animation
geom.h - contains structs to store geometrical objects

## Tests
Various test cases can be implemented by the user by drawing the polygon and placing the security guard in the desire position.


## Compile and Run
compile: run 'make' from the command line to compile
clean: run 'make clean'

run: ./plan

test: use the following keypress features to test the code:
	q: quit
	s: clear the screen to draw a new polygon
	g: done drawing the polygon, ready to chose the guard
	a: toggle animation
	z: erase the most recently drawn edge of the polygon
