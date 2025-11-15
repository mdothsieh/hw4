#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelper(Node* root, int depth, int& leafDepth);

bool equalPathsHelper(Node* root, int depth, int& leafDepth){

  if(root==nullptr){ // tree is empty
    return true;
  }

  if(root->left == nullptr && root->right == nullptr){// node is leaf
    if(leafDepth == -1){// first leaf found
      leafDepth = depth; // keep track of depth
      return true;
    }
    return depth == leafDepth; // compare current depth with first leaf depth
  }

  bool leftCheck = true; // recursively check left subtree
  bool rightCheck = true; // recursively check right subtree
  if(root->left != nullptr){//check left child
    leftCheck = equalPathsHelper(root->left, depth + 1, leafDepth);
  }

  if(root->right != nullptr){//check right child
    rightCheck = equalPathsHelper(root->right, depth + 1, leafDepth);
  }

  return (leftCheck && rightCheck); // both subtrees must be equal

}


bool equalPaths(Node * root)
{
    // Add your code below
    int leafDepth = -1; // initialize leaf depth
    return equalPathsHelper(root, 0, leafDepth); // recursion from root at depth 0

}

