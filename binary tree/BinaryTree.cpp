#include "BinaryTree.h"
#include <iostream>

using namespace std;

#define _LEAKCHECK
#ifdef _LEAKCHECK
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#undef _LEAKCHECK

// Mert Ekici 26772 CS204 HW3

//Constructor
BinaryTree::BinaryTree(){
  root = nullptr;
}

//Copy Constructor
BinaryTree::BinaryTree(const BinaryTree& rhs){
  //TODO: Implement deep copy constructor
  //You may make use of Iterator class
	
	root = nullptr;
	
	if(rhs.root){
		Iterator i;
		i.Init(rhs);
		while (i.hasNext()){
			insert(i.Next()->value);
		}
	}
}

//Destructor
BinaryTree::~BinaryTree(){
  //TODO: Implement destructor

	if(root){
		Iterator i;
		i.Init(*this);
		while (i.hasNext()){
			delete i.Next();
		}

		//memory leak check
#ifdef _LEAKCHECK
		cout<<"bt: "<<_CrtDumpMemoryLeaks()<<endl;
#endif
	}
}

void BinaryTree::insert(int num) {
  //TODO: Implement insert function for BinaryTree


	if(!root){
		TreeNode* nn = new TreeNode;
		nn->value = num;
		root = nn;
		return;
	}
	else{
		TreeNode* currN = root;
		while(currN){
			if ( num < currN->value ){
				if(currN->left){
					currN = currN->left;
				}
				else{
					TreeNode* nn = new TreeNode;
					nn->value = num;
					currN->left = nn;
					return;

				}
			}
			else if( num > currN->value ){
				if(currN->right){
					currN = currN->right;
				}
				else{
					TreeNode* nn = new TreeNode;
					nn->value = num;
					currN->right = nn;
					return;
				}
			}
			else{ //if num == currN->value
				cout<<"Duplicate value found in tree."<<endl;
				return;
			}
		}
		
	}
	
	
	
	
}

//Operators
//Give the implementations of operators in here either as free function or member function


//output stream operator
ostream & operator <<(ostream & os, const BinaryTree& tree){

	Iterator i;
	i.Init(tree);
	while (i.hasNext())
		os << i.Next()->value << " ";
	return os<<endl; // endl for clear outputs
}

//adds rhs nodes to the current tree
const BinaryTree & BinaryTree::operator += (const BinaryTree & rhs){
	
	if(rhs.root){
		Iterator i;
		i.Init(rhs);
		while (i.hasNext())
			insert(i.Next()->value);
	}

	return *this;

}

//inserts num to the current tree
const BinaryTree & BinaryTree::operator += (int num){
	
	insert(num);
	return *this;

}

//asignment operator
BinaryTree & BinaryTree::operator = (const BinaryTree & rhs){

	if(this != &rhs){
		//delete current
		if(root){
			Iterator i;
			i.Init(*this);
			while (i.hasNext()){
				delete i.Next();
			}
		}
		//make deep copy
		root = nullptr;

		if(rhs.root){

			Iterator i;
			i.Init(rhs);
			while (i.hasNext()){
				insert(i.Next()->value);
			}
		}
	}
	return *this;
}

//+ operator
BinaryTree operator+ (const BinaryTree & lhs,const BinaryTree & rhs) {
	BinaryTree result(lhs);
	result += rhs;
	return result;
}
//lhs tree, rhs int + operator
BinaryTree  operator+ (const BinaryTree & lhs,int num) {
	BinaryTree result(lhs);
	result.insert(num);
	return result;
}
//lhs int, rhs tree + operator
BinaryTree operator+ (int num,const BinaryTree & rhs) {
	BinaryTree result(rhs);
	result.insert(num);
	return result;
}

//equality check function for both have same elements, helper to "==" "!=" operators
bool BinaryTree::Equals(const BinaryTree & bt) const{

	if ( !root && !bt.root ){
		return true;
	} 
	Iterator ibt;
	ibt.Init(bt);
	Iterator myi;
	myi.Init(*this);
	while(ibt.hasNext() && myi.hasNext()){
		if(ibt.Next()->value != myi.Next()->value){
			return false;
		}
	}
	if(ibt.Next()!=myi.Next()){  //if one of the trees is out of node check the other one
		return false;
	}
	return true;
}

//== operator returns lhs equals rhs
bool operator ==(const BinaryTree & lhs,const BinaryTree & rhs){
	return lhs.Equals(rhs); 
}

//!= operator returns lhs not equal rhs
bool operator !=(const BinaryTree & lhs,const BinaryTree & rhs){
	return !lhs.Equals(rhs);
}
/*
 *  ITERATOR CLASS
 */

//Constructor
Iterator::Iterator()
  :myCurrent(nullptr), stack(nullptr)
{}
//Destructor for leaked stack elements of the iterator class
Iterator::~Iterator(){
	while(!stack->isEmpty()){
		stack->pop();
	}
	delete stack;

#ifdef _LEAKCHECK
	cout<<"it: "<<_CrtDumpMemoryLeaks()<<endl;
#endif
}

void Iterator::Init(const BinaryTree &tree)
{
  myCurrent = tree.root;
  stack = new Stack();

  while (myCurrent){
		stack->push(myCurrent);
		myCurrent = myCurrent->left;
	}
}

//Returns true if tree has more nodes to explore
bool Iterator::hasNext()
{
  //TODO: Implement a function to see if there is more nodes to iterate over.
	return !stack->isEmpty();
}

//Next node to explore
TreeNode* Iterator::Next()
{
  //TODO: Implement next function of the iterator
  //Note that it returns the next element
	
	TreeNode* node = stack->pop();
	if(node){
		myCurrent = node->right;
		while (myCurrent){
			stack->push(myCurrent);
			myCurrent = myCurrent->left;
		}
	}
	return node;
}


