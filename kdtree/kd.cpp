/*  kd.cpp
*
*  builds a two-dimensional kd-tree for a set of points in a plane.
*
*  created by bridget went and claire mccarthy 3/18/17
*  last modified 3/28/17
*
*/


#include "kd.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>


using namespace std;

/* default constructor builds an empty tree */
KdTree::KdTree() {
  this->root = NULL;
  this->count = 0;
  this->h = 0;
}

/* Build a kd-tree for the set of n points, where each leaf cell
contains  1 point.
*/
KdTree::KdTree(Point2D p[], int n ) {

  // allocate, copy, and sort points by x-coord
  Point2D points_by_x[n];
  for (int i = 0; i < n; ++i) {
    points_by_x[i] = p[i];
  }
  qsort(points_by_x, n, sizeof(Point2D), &leftToRightCmp);

  // allocate, copy, and sort points by y-coord
  Point2D points_by_y [n];
  for (int i = 0; i < n; ++i) {
    points_by_y[i] = p[i];
  }
  qsort(points_by_y, n, sizeof(Point2D), &bottomToTopCmp);

  // printf("points sorted by x: \n");
  // for (int i = 0; i < n; ++i) {
  //   printf("[%f %f]\n", points_by_x[i].x, points_by_x[i].y);
  // }
  //
  // printf("\npoints sorted by y: \n");
  // for (int i = 0; i < n; ++i) {
  //   printf("[%f %f]\n", points_by_y[i].x, points_by_y[i].y);
  // }

  printf("\n\nbuilding kd tree...\n");

  // build the kd tree recursively using sorted arrays
  root = buildKdtree(points_by_x, points_by_y, n, 0);

  // calculate the height of the tree starting at the root
  h = height(root);

  printf("-----------------\ndone.\n");

}

/* destructor to free all nodes in the kd tree, starting at the root */
KdTree::~KdTree() {
  printf("freeing kd tree...\n");
  free_node(root);
  printf("-----------------\ndone.\n");
}

/* recursive function to free all nodes in the kd tree */
void KdTree::free_node(TreeNode *node) {
  if (node == NULL) {
    return;
  }

  // free left and right nodes first
  free_node(node->left);
  free_node(node->right);
  free(node);

}

/*  helper function to recursively build the kd tree given sorted copies of P sorted
by x and y coordinate. returns a pointer to the root of the tree */
TreeNode* KdTree::buildKdtree(Point2D points_by_x[], Point2D points_by_y[], int n, int depth) {

  /* base case: if there is only one point, return a leaf storing this point */
  if (n <= 1) {
    TreeNode *leaf = new TreeNode(points_by_x[0], LEAF);
    /* leaf nodes have no left or right children*/
    leaf->left = NULL;
    leaf->right = NULL;
    return leaf;
  }

  // set the median
  int median = n % 2 == 0 ? n/2 : n/2 + 1;

  /* allocate arrays to use while splitting points along the  median */
  Point2D p1_x[median]; // left/bottom points, sorted by x (smaller x or y coord, including median)
  Point2D p1_y[median]; // left/bottom points, sorted by y
  Point2D p2_x[n/2]; // right/top points, sorted by x (larger x or y coord, not including median)
  Point2D p2_y[n/2]; // right/top points, sorted by y

  // make a new node
  TreeNode *v;

  /* if depth is even, split points with vertical line through median x-coord
  into p1 (left of or on the line) and p2 (right of the line) */
  if (depth % 2 == 0) {

    /* initialize new node */
    v = new TreeNode(points_by_x[median - 1], VERTICAL);

    /* split the points into P1, the first half */
    for (int i = 0; i < median; ++i) {
      p1_x[i] = points_by_x[i];
      p1_y[i] = points_by_x[i];
    }
    /* sort by y coord */
    qsort(p1_y, median, sizeof(Point2D), &bottomToTopCmp);

    /* split the points into P2, the second half */
    int i = 0;
    for (int j = median; j < n; ++j) {
      p2_x[i] = points_by_x[j];
      p2_y[i] = points_by_x[j];
      ++i;
    }
    /* sort by y coord */
    qsort(p2_y, n/2, sizeof(Point2D), &bottomToTopCmp);

  } else {
    /* if depth is odd, split points with horizontal line through median y-coord
    into P1 (below or on the line) and P2 (above the line) */

    /* initialize new node */
    v = new TreeNode(points_by_y[median - 1], HORIZONTAL);

    /* split points into P1, the first half */
    for (int i = 0; i < median; ++i) {
      p1_y[i] = points_by_y[i];
      p1_x[i] = points_by_y[i];
    }
    /* sort by x coord */
    qsort(p1_x, median, sizeof(Point2D), &leftToRightCmp);


    /* split points into P2, the second half */
    int i = 0;
    for (int j = median; j < n; ++j) {
      p2_y[i] = points_by_y[j];
      p2_x[i] = points_by_y[j];
      ++i;
    }
    /* sort by x coord */
    qsort(p2_x, n/2, sizeof(Point2D), &leftToRightCmp);

  }

  /* recursively build the left and right subtrees */
  v->left = buildKdtree(p1_x, p1_y, median, depth + 1);
  v->right = buildKdtree(p2_x, p2_y, n - median, depth + 1);

  count++;

  return v;

}

/* calculates the height of the tree recurisvely */
int KdTree::height(TreeNode *node) {
  if (node == NULL) return 1;

  /* compare heights of left and right subtrees and return larger */
  int h1 = 1 + height(node->left);
  int h2 = 1 + height(node->right);

  if (h1 > h2) return h1;
  return h2;

}

/* print out info about the kd tree and its nodes */
void KdTree::kd_print() {
    printf("\n---------kd tree info -----------\n");
    printf("height: %d, number of nodes: %d\n", h, count);
    printf("nodes in order:\n\n");
    treeNode_print(root);
    printf("\n");

}

/* pre order traversal to print nodes */
void treeNode_print(TreeNode *node) {
  if (node == NULL) return;

  treeNode_print(node->left);
  treeNode_print(node->right);

  switch(node->cuttype) {
    case HORIZONTAL:
    printf("HORIZONTAL: (y=%f)\n", node->p.y);
    break;
    case VERTICAL:
    printf("VERTICAL: (x=%f)\n", node->p.x);
    break;
    case LEAF:
    printf("LEAF: (p=(%f,%f))\n", node->p.x, node->p.y);
    break;
    default:
    printf("invalid node cuttype\n");
    exit(1);
  };


}


/*  (from assignemnt page) These comparators uniquely order points by comparing both
on x- and y-coordinates, and unless there are coincident points, no two points
are equal under these compare functions.
All points before the median are strictly smaller than the median, so they all go to the left;
same for the points after the median. There is no need to handle degenerate cases,
because under these compare functions there are none: all points are unique.
*/

//orders the points by x, and for same x in y-order
int leftToRightCmp(const void *a, const void *b) {
  const Point2D p1 = *(const Point2D*)a;
  const Point2D p2 = *(const Point2D*)b;
  if (p1.x != p2.x) {
    return (p1.x - p2.x);
  } else {
    return (p1.y - p2.y);
  }
}

//orders the points by y, and for same y in x-order
int bottomToTopCmp(const void *a, const void *b) {
  const Point2D p1 = *(const Point2D *)a;
  const Point2D p2 = *(const Point2D *)b;
  if (p1.y != p2.y) {
    return (p1.y - p2.y);
  } else {
    return (p1.x - p2.x);
  }

}
