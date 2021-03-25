#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include "Stack.h"

using namespace std;

// Mert Ekici 26772 CS204 HW3

class BinaryTree
{
 public:
  //Constructor
  BinaryTree();
  //Copy Constructor - TODO in .cpp file
  BinaryTree(const BinaryTree&);
  //Destructor
  ~BinaryTree();
  
  // Insertion method
  void insert(int);

  // Operators
  friend ostream & operator <<(ostream & os, const BinaryTree& tree);
  const BinaryTree & operator += (const BinaryTree & rhs);
  const BinaryTree & operator += (int num);
  BinaryTree & operator = (const BinaryTree & rhs);
  bool Equals(const BinaryTree & bt) const;
  
private:
  //The root of the tree
  TreeNode* root;

  friend class Iterator;
};

class Iterator{
public:
  //Constructor
  Iterator();
  //Destructor
  ~Iterator();
  
  void Init(const BinaryTree& );
  bool hasNext();
  TreeNode* Next();
  
private:
  TreeNode* myCurrent;
  Stack* stack;
};

//free operators
BinaryTree operator+ (const BinaryTree & lhs,const BinaryTree & rhs);
BinaryTree  operator+ (const BinaryTree & lhs,int num);
BinaryTree operator+ (int num,const BinaryTree & rhs);
bool operator ==(const BinaryTree & lhs,const BinaryTree & rhs);
bool operator !=(const BinaryTree & lhs,const BinaryTree & rhs);

#endif
