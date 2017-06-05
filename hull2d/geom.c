/*  geom.c
 *
 *  graham scan algorithm for finding convex hull on a set of points
 *
 *  created by bridget went and claire mccarthy 2/6/17
 *  last modified 2/9/17
 *
 */


#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const double LARGE = 9.9e5;
static const double EPSILON = 1e-10;

/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c) {
  return ((b.x - a.x) * (c.y - a.y) -
            (c.x - a.x) * (b.y - a.y));

            //double dArea = ((dX1 - dX0)*(dY2 - dY0) - (dX2 - dX0)*(dY1 - dY0))/2.0;
}
//CLAIRE 2/6 4pm


/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  return (signed_area2D(p, q, r) == 0);
}
//CLAIRE 2/6 4pm


/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) {
  return (signed_area2D(a, b, c) > 0);
}
//CLAIRE 2/6 4pm

/* swaps 2 points a and b */
void swap (point2D *a, point2D *b) {
  point2D tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}
// BRIDGET 2/7

/* returns the distance squared between 2 points p1 and p2*/
int dist(point2D p1, point2D p2)
{
    return (p1.x - p2.x)*(p1.x - p2.x) +
          (p1.y - p2.y)*(p1.y - p2.y);
}
// BRIDGET 2/7


/* comparison function for sorting by ccw angle */
int compare(const void *a, const void *b) {
  point2D *p1 = (point2D *)a;
  point2D *p2 = (point2D *)b;

  // handle case where points are collinear
   if (collinear(*p1, *P, *p2)) {
      return (dist(*P, *p2) >= dist(*P, *p1))? -1 : 1;

   }

   // if p2 is to the left of the line from P to p1, it must be behind p1 in
   // sorted order
   return (signed_area2D(*p1, *P, *p2) < 0)? -1: 1;
}
// BRIDGET 2/7

/* STACK IMPLEMENTATION using a linked list
push a pointNode onto the stack, given pointer to its head
*/
pointNode* push(pointNode* head,point2D a)
{
    pointNode* tmp = (pointNode*)malloc(sizeof(pointNode *));
    if(tmp == NULL)
    {
        exit(0);
    }
    tmp->p = a;
    tmp->next = head;
    head = tmp;
    return head;
}
// BRIDGET 2/7

/* STACK IMPLEMENTATION using a linked list
pop a pointNode from the stack, given pointer to its head
*/
pointNode* pop(pointNode *head)
{
    pointNode* tmp = head;
    head = head->next;
    free(tmp);
    return head;
}
// BRIDGET 2/7

/* FOR DEBUGGING: print out points and the angles they make with P */
void printer(point2D *p, int n) {
  for (int i = 0; i < n; ++i) {
    double angle = atan2((double)(p[i].y - P->y), (double)(p[i].x - P->x));
    printf("(%d, %d) angle: %f\n", p[i].x, p[i].y, angle);
  }
  printf("\n");
}
// BRIDGET 2/8

/* compute the convex hull of the points in p; the points on the CH are returned as a list
*/
pointNode* graham_scan(point2D *p, int n) {
  pointNode *head = NULL;

  // handle the case where only 3 points on CH
  if (n <= 3) {
    for (int i = 0; i < n; ++i) {
      head = push(head, p[i]);
    }
    return head;
  }

  //find the point with the lowest y value
  int minY_index = 0;

  // initialize P
  P = (point2D *)malloc(sizeof(point2D*));
  P->x = 0.0;
  P->y = LARGE;

  // find point with min y value and set it to P (must be on CH)
  for (int i = 0; i < n; i++) {
    if (p[i].y < P->y) {
      minY_index = i;
      P->y = p[i].y;
    }

  }
  P->x = p[minY_index].x;
  P->y = p[minY_index].y;

  // put P at the head of the list of points
  swap(&p[0], &p[minY_index]);

  // sort the points in ccw order using quicksort
  qsort(&p[1], n-1, sizeof(point2D*), compare);

  // add the first 3 points onto the convex hull
  head = push(head, p[0]);
  head = push(head, p[1]);
  head = push(head, p[2]);
  int count = 3;

  // traverse all points and maintain the convex hull of all traversed points
  for (int i = 3; i < n; i++) {

    // if the point is to the left of the previous 2 points, add to the CH
    if (left(head->next->p, head->p, p[i])) {
      head = push(head,p[i]);
      count++;
    } else {
      // repeat popping from he stack while the point is to the right of the top
      // 2 points on the CH
      while (count > 2 && head->next != NULL && !left(head->next->p, head->p, p[i])) {
        head = pop(head);
        count--;
      }

      // then add to the CH
      head = push(head, p[i]);
      count++;
    }
  }

  return head;
}
//CLAIRE 2/6 4pm
// BRIDGET 2/8
