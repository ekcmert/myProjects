#ifndef _STACK_H
#define _STACK_H

#include <iostream>

// Mert Ekici 26772 CS204 HW3

struct TreeNode
{
  int value;
  TreeNode *left;
  TreeNode *right;

  TreeNode(){
    left = nullptr;
    right = nullptr;
  }
};

struct StackNode
{
  TreeNode* value;
  StackNode *next;
};

class Stack
{
private:
  StackNode *top;
  StackNode* GetTopPointer(Stack myStack);
  
public:
  Stack(void);
  void push(TreeNode* );
  TreeNode* pop();
  bool isEmpty();
};

#endif
