
#ifndef __kd_h
#define __kd_h

struct Point2D {
  double x, y;
};

enum Type {
  HORIZONTAL,
  VERTICAL,
  LEAF
};

const int WINDOWSIZE = 500;

struct TreeNode {

  Point2D p; /* If this is a leaf node,  p represents the point stored in this leaf.
  If this is not a leaf node,  p represents the horizontal or vertical line
  stored in this node. For a vertical line, p.y is
  ignored. For a horizontal line, p.x is ignored.
  */
  Type cuttype;
  /* this can be HORIZONTAL, VERTICAL, or LEAF
  depending whether the node splits with a horizontal line or  vertical line.
  */
  TreeNode *left, *right; /* left/below and right/above children. */

  TreeNode(Point2D point, Type axis) {
    cuttype = axis;
    p = point;
  }

  ~TreeNode() { /* FILL IN */
    delete left;
    delete right;

   };
};

class KdTree {
private:
  TreeNode* buildKdtree(Point2D points_by_x[], Point2D points_by_y[], int n, int depth);
  int count; // number of nodes in the tree
  int h; // height of the tree
  TreeNode* root; // root of the kd tree
  void free_node(TreeNode *node);
  int height(TreeNode *node);

public:
  KdTree();
  KdTree(Point2D p[], int n );
  void kd_print();
  TreeNode* getRoot() { return root; };
  ~KdTree();

};

int leftToRightCmp(const void *a, const void *b);
int bottomToTopCmp(const void *a, const void *b);
void treeNode_print(TreeNode *node);
TreeNode* createNode(Point2D points[], int n);
void treeNode_print(TreeNode *node);
// int height(TreeNode *t);
// int Height(KdTree* tree);


#endif
