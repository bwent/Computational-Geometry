## Synopsis

This program is designed to compute the convex hull of a set of points in 3D using the Gift Wrapping algorithm. 

*** NOTE: not working with keypress events as of 4/2/17 ***

## Authors

Written by Claire McCarthy and Bridget Went for Computational Geometry, Spring 2017.
Last modified 3/30/2017.

## Code
geom.cpp - code to implement Gift Wrapping algorithm to compute the CH
geom.h - header file for CH

viewpoints.cpp - GL code to display points and their CH, implement test cases

## Tests

Various tests were generated by students in the class and integrated into the code.
These test cases provide different sets of points to test the accuracy of the convex hull.


## Compile and Run
compile: run 'make' from the command line to compile

run: ./hull3d <number of points>

test: toggle between test cases by pressing letters on the keyboard. The following letters implement the following test cases: 
      i: random
      j: pyramid
      k: cross
      m: diamond
      n: spring
      p: sphere of spheres
      s: random vertical lines
      t: heart
      w: droplet
      