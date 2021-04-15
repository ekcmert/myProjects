#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;
//CS300 HW2 BST - AVL Trees
//Mert Ekici 26772 

// TREE IMPLEMENTATIONS

//BST

//class declaration
template <class Comparable>
class BinarySearchTree;

template <class Comparable>
class BinaryNode
{
public:
	Comparable element;
	BinaryNode  *left;
	BinaryNode  *right;

	BinaryNode( const Comparable & theElement, 
		BinaryNode *lt, BinaryNode *rt )
		: element( theElement ), left( lt ), right( rt ) { }

	friend class BinarySearchTree<Comparable>;
};

template <class Comparable>
class BinarySearchTree
{
public:
	explicit BinarySearchTree( const Comparable & notFound );
	BinarySearchTree( const BinarySearchTree & rhs );
	~BinarySearchTree( );

	const Comparable & findMin( ) const;
	const Comparable & findMax( ) const;
	const Comparable & find( const Comparable & x ) const;
	bool isEmpty( ) const;
	void printTree( ) const;

	void makeEmpty( );
	void insert( const Comparable & x );
	void remove( const Comparable & x );

	const BinarySearchTree & operator=( const BinarySearchTree & rhs );

	BinaryNode<Comparable> *root;
	const Comparable ITEM_NOT_FOUND;

	const Comparable & elementAt( BinaryNode<Comparable> *t ) const;

	void insert( const Comparable & x, BinaryNode<Comparable> * & t ) const;
	void remove( const Comparable & x, BinaryNode<Comparable> * & t ) const;
	BinaryNode<Comparable> * findMin( BinaryNode<Comparable> *t ) const;
	BinaryNode<Comparable> * findMax( BinaryNode<Comparable> *t ) const;
	BinaryNode<Comparable> * find( const Comparable & x, 
		BinaryNode<Comparable> *t ) const;
	void makeEmpty( BinaryNode<Comparable> * & t ) const;
	void printTree( BinaryNode<Comparable> *t ) const;
	BinaryNode<Comparable> * clone( BinaryNode<Comparable> *t ) const;
};

//member functions
template <class Comparable>
BinarySearchTree<Comparable>::
	BinarySearchTree(const Comparable & notFound ) :
	ITEM_NOT_FOUND( notFound ), root( NULL )
{
}

template <class Comparable>
bool BinarySearchTree<Comparable>::isEmpty( ) const{
	return root == NULL;
}

template <class Comparable>
const Comparable & BinarySearchTree<Comparable>::
	elementAt( BinaryNode<Comparable> *t ) const
{
	return t == NULL ? ITEM_NOT_FOUND : t->element;
}

template <class Comparable>
const Comparable & BinarySearchTree<Comparable>::
	find( const Comparable & x ) const
{
	return elementAt( find( x, root ) );
}

template <class Comparable>
BinaryNode<Comparable> *
	BinarySearchTree<Comparable>::
	find( const Comparable & x, BinaryNode<Comparable> * t ) const
{
	if ( t == NULL )
		return NULL;
	else if( x < t->element )
		return find( x, t->left );
	else if( t->element < x )
		return find( x, t->right );
	else
		return t;    // Match
}

template <class Comparable>
const Comparable & BinarySearchTree<Comparable>::findMin( ) const
{
	return elementAt( findMin( root ) );
}
template <class Comparable>
BinaryNode<Comparable> *
	BinarySearchTree<Comparable>::findMin( BinaryNode<Comparable> *t ) 
	const
{
	if( t == NULL )
		return NULL;
	if( t->left == NULL )
		return t;
	return findMin( t->left );
}

template <class Comparable>
const Comparable & BinarySearchTree<Comparable>::findMax( ) const
{
	return elementAt( findMax( root ) );
}

template <class Comparable>
BinaryNode<Comparable> *
	BinarySearchTree<Comparable>::findMax( BinaryNode<Comparable> *t ) 
	const
{
	if( t != NULL )
		while( t->right != NULL )
			t = t->right;
	return t;
}

template <class Comparable>
void BinarySearchTree<Comparable>::insert( const Comparable & x )
{
	insert( x, root );
}

template <class Comparable>
void BinarySearchTree<Comparable>::
	insert( const Comparable & x, BinaryNode<Comparable> * & t ) const
{
	BinaryNode<Comparable>* curr = t;
	BinaryNode<Comparable>* parent = NULL;
	if (root == NULL)
	{
		t = new BinaryNode<Comparable>( x, NULL, NULL );
		return;
	}
	while(curr!= NULL){
		parent = curr;
		if( x < curr->element )
			curr = curr->left;
		else if( curr->element < x )
			curr = curr->right;
		else
			return;
	}
	if (x < parent->element){
		parent->left =  new BinaryNode<Comparable>( x, NULL, NULL );
	}
	else{
		parent->right=  new BinaryNode<Comparable>( x, NULL, NULL );
	}


}

template <class Comparable>
void BinarySearchTree<Comparable>::remove
	( const Comparable & x )
{
	remove( x, root );
}

template <class Comparable>
void BinarySearchTree<Comparable>::remove( const Comparable & x, BinaryNode<Comparable> * & t ) const
{
	if( t == NULL )
		return;   // Item not found; do nothing
	if( x < t->element )
		remove( x, t->left );
	else if( t->element < x )
		remove( x, t->right );
	else if( t->left != NULL && t->right != NULL ) // Two children
	{
		t->element = findMin( t->right )->element;
		remove( t->element, t->right );
	}
	else // one or no children
	{
		BinaryNode<Comparable> *oldNode = t;
		t = ( t->left != NULL ) ? t->left : t->right;
		delete oldNode;
	}
}         

template <class Comparable>
void BinarySearchTree<Comparable>::makeEmpty( )
{
	makeEmpty( root );
}


template <class Comparable>
void BinarySearchTree<Comparable>::
	makeEmpty( BinaryNode<Comparable> * & t ) const
{
	if( t != NULL )
	{
		makeEmpty( t->left );
		makeEmpty( t->right );
		delete t;
	}
	t = NULL;
}

template <class Comparable>
void BinarySearchTree<Comparable>::printTree( ) const
{
	if( isEmpty( ) )
		return;
	else
		printTree( root );
}

template <class Comparable>
void BinarySearchTree<Comparable>::
	printTree( BinaryNode<Comparable> * t ) const
{
	if ( t != NULL )
	{
		printTree( t->left );
		cout << t->element << endl;
		printTree( t->right );
	}
}


template <class Comparable>
const BinarySearchTree<Comparable> &  BinarySearchTree<Comparable>::operator=( const BinarySearchTree<Comparable> & rhs ){
	if (this != &rhs){
		makeEmpty();
		root = rhs.clone(rhs.root);

	}
	return *this;
}

template <class Comparable>
BinaryNode<Comparable> *
	BinarySearchTree<Comparable>::
	clone( BinaryNode<Comparable> * t ) const
{
	if ( t == NULL )
		return NULL;
	else
		return new BinaryNode<Comparable> 
		( t->element, 
		clone( t->left ), 
		clone( t->right ) );
}


template <class Comparable>
BinarySearchTree<Comparable>::~BinarySearchTree()
{
	makeEmpty( );
}

template <class Comparable>
BinarySearchTree<Comparable>::BinarySearchTree( const BinarySearchTree<Comparable> & rhs ) : root( NULL ), ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND )
{ 
	*this = rhs;
}


//AVL

//class declaration
template <class Comparable>
class AvlTree;

template <class Comparable>
class AvlNode
{
public:
	Comparable 	element;
	AvlNode   		*left;
	AvlNode   		*right;
	int        		height;

	AvlNode( const Comparable & theElement, 
		AvlNode *lt, AvlNode *rt, int h = 0 )
		: element( theElement ), left( lt ), right( rt ), height( h ) { }

	friend class AvlTree<Comparable>;
};

template <class Comparable>
class AvlTree
{
public:
	explicit AvlTree( const Comparable & notFound );
	AvlTree( const AvlTree & rhs );
	~AvlTree( );

	const Comparable & findMin( ) const;
	const Comparable & findMax( ) const;
	const Comparable & find( const Comparable & x ) const;
	bool isEmpty( ) const;
	void printTree( ) const;
	void printTreeHeight( ) const;

	void makeEmpty( );
	void insert( const Comparable & x );
	void remove( const Comparable & x );

	const AvlTree & operator=( const AvlTree & rhs );
	AvlNode<Comparable> *root;
	const Comparable ITEM_NOT_FOUND;

	const Comparable & elementAt( AvlNode<Comparable> *t ) const;

	void insert( const Comparable & x, AvlNode<Comparable> * & t ) const;
	void remove(const Comparable & x, AvlNode<Comparable> * & t ) const;

	AvlNode<Comparable> * findMin( AvlNode<Comparable> *t ) const;
	AvlNode<Comparable> * findMax( AvlNode<Comparable> *t ) const;
	AvlNode<Comparable> * find( const Comparable & x, AvlNode<Comparable> *t ) const;
	void makeEmpty( AvlNode<Comparable> * & t ) const;
	void printTree( AvlNode<Comparable> *t ) const;
	void printTreeHeight( AvlNode<Comparable> *t ) const;
	AvlNode<Comparable> * clone( AvlNode<Comparable> *t ) const;

	// Avl manipulations
	int height( AvlNode<Comparable> *t ) const;
	int max( int lhs, int rhs ) const;
	void rotateWithLeftChild( AvlNode<Comparable> * & k2 ) const;
	void rotateWithRightChild( AvlNode<Comparable> * & k1 ) const;
	void doubleWithLeftChild( AvlNode<Comparable> * & k3 ) const;
	void doubleWithRightChild( AvlNode<Comparable> * & k1 ) const;
};


//member functions
template <class Comparable>
AvlTree<Comparable>::
	AvlTree(const Comparable & notFound ) :
	ITEM_NOT_FOUND( notFound ), root( NULL )
{
}


template <class Comparable>
bool AvlTree<Comparable>::isEmpty( ) const{
	return root == NULL;
}

template <class Comparable>
const Comparable & AvlTree<Comparable>::
	elementAt( AvlNode<Comparable> *t ) const
{
	return t == NULL ? ITEM_NOT_FOUND : t->element;
}

template <class Comparable>
const Comparable & AvlTree<Comparable>::
	find( const Comparable & x ) const
{
	return elementAt( find( x, root ) );
}

template <class Comparable>
AvlNode<Comparable> *
	AvlTree<Comparable>::
	find( const Comparable & x, AvlNode<Comparable> * t ) const
{
	if ( t == NULL )
		return NULL;
	else if( x < t->element )
		return find( x, t->left );
	else if( t->element < x )
		return find( x, t->right );
	else
		return t;    // Match
}

template <class Comparable>
const Comparable & AvlTree<Comparable>::findMin( ) const
{
	return elementAt( findMin( root ) );
}
template <class Comparable>
AvlNode<Comparable> *
	AvlTree<Comparable>::findMin( AvlNode<Comparable> *t ) 
	const
{
	if( t == NULL )
		return NULL;
	if( t->left == NULL )
		return t;
	return findMin( t->left );
}

template <class Comparable>
const Comparable & AvlTree<Comparable>::findMax( ) const
{
	return elementAt( findMax( root ) );
}

template <class Comparable>
AvlNode<Comparable> *
	AvlTree<Comparable>::findMax( AvlNode<Comparable> *t ) const
{
	if( t != NULL )
		while( t->right != NULL )
			t = t->right;
	return t;
}


template <class Comparable>
int AvlTree<Comparable>::height( AvlNode<Comparable> *t ) 
	const
{
	if (t == NULL)
		return -1;

	return t->height;
}
template <class Comparable>
int AvlTree<Comparable>::max( int lhs, int rhs ) 
	const
{
	if (lhs > rhs)
		return lhs;

	return rhs;            
}

template <class Comparable>
void AvlTree<Comparable>::insert( const Comparable & x )
{
	insert( x, root );
}

template <class Comparable>
void AvlTree<Comparable>::insert
	( const Comparable & x, AvlNode<Comparable> * & t ) const
{
	if ( t == NULL )
		t = new AvlNode<Comparable>( x, NULL, NULL );

	else if ( x < t->element ) {
		// X should be inserted to the left tree!
		insert( x, t->left );

		// Check if the left tree is out of balance (left subtree grew in height!)
		if ( height( t->left ) - height( t->right ) == 2 )
			if ( x < t->left->element )  // X was inserted to the left-left subtree!
				rotateWithLeftChild( t );
			else			     // X was inserted to the left-right subtree!
				doubleWithLeftChild( t );
	}
	else if( t->element < x )
	{    // Otherwise X is inserted to the right subtree
		insert( x, t->right );
		if ( height( t->right ) - height( t->left ) == 2 )
			// height of the right subtree increased
				if ( t->right->element < x )
					// X was inserted to right-right subtree
						rotateWithRightChild( t );
				else // X was inserted to right-left subtree
					doubleWithRightChild( t );
	}
	else
		;  // Duplicate; do nothing

	// update the height the node
	t->height = max( height( t->left ), height( t->right ) ) + 1;
}

template <class Comparable>
void AvlTree<Comparable>::rotateWithLeftChild( 
	AvlNode<Comparable> * & k2 ) const
{
	AvlNode<Comparable> *k1 = k2->left;
	k2->left = k1->right;
	k1->right = k2;
	k2->height = max( height( k2->left ), height( k2->right ) ) + 1;
	k1->height = max( height( k1->left ), k2->height ) + 1;
	k2 = k1;
}

template <class Comparable>
void AvlTree<Comparable>::rotateWithRightChild
	( AvlNode<Comparable> * & k1 ) const
{
	AvlNode<Comparable> *k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;
	k1->height = max( height( k1->left ), height( k1->right ) ) + 1;
	k2->height = max( height( k2->right ), k1->height ) + 1;
	k1 = k2;
}

template <class Comparable>
void AvlTree<Comparable>::doubleWithLeftChild
	( AvlNode<Comparable> * & k3 ) const
{
	rotateWithRightChild( k3->left );
	rotateWithLeftChild( k3 );
}

template <class Comparable>
void AvlTree<Comparable>::doubleWithRightChild
	( AvlNode<Comparable> * & k1 ) const
{
	rotateWithLeftChild( k1->right );
	rotateWithRightChild( k1 );
}

template <class Comparable>
void AvlTree<Comparable>::remove( const Comparable & x ){
	remove( x, root );
}

template <class Comparable>
void AvlTree<Comparable>::remove(const Comparable & x, AvlNode<Comparable> * & t ) const{
	if( t == NULL )
		return;   // Item not found; do nothing
	if( x < t->element ){
		remove( x, t->left );
		if ( height( t->right ) - height( t->left ) == 2 ){
			// height of the right subtree increased
			if ( t->left == NULL)
					rotateWithRightChild( t );
			else {
				if(t->right->right->height < t->right->left->height){
					doubleWithRightChild( t );}
				else{
					rotateWithRightChild( t );
				}
			}
		}
	}
	else if( t->element < x ){
		remove( x, t->right );
		if ( height( t->left ) - height( t->right ) == 2 ){
			if ( t->right == NULL )  
				rotateWithLeftChild( t );
			else{     
				if(t->left->left->height < t->left->right->height){
					doubleWithLeftChild( t );}
				else{
					rotateWithLeftChild( t );
				}
			}
		}
	}
	else if( t->left != NULL && t->right != NULL ) // Two children
	{
		t->element = findMin( t->right )->element;
		remove( t->element, t->right );
		if ( height( t->left ) - height( t->right ) == 2 )
			rotateWithLeftChild( t );

	}
	else // one or no children
	{
		AvlNode<Comparable> *oldNode = t;
		t = ( t->left != NULL ) ? t->left : t->right;
		delete oldNode;
	}

	// update the height the node
	if(t!=NULL)
		t->height = max( height( t->left ), height( t->right ) ) + 1;
}

template <class Comparable>
void AvlTree<Comparable>::makeEmpty()
{
	makeEmpty( root );
}

template <class Comparable>
void AvlTree<Comparable>::
	makeEmpty( AvlNode<Comparable> * & t ) const
{
	if( t != NULL )
	{
		makeEmpty( t->left );
		makeEmpty( t->right );
		delete t;
	}
	t = NULL;
}

template <class Comparable>
void AvlTree<Comparable>::printTree( ) const
{
	if( isEmpty( ) )
		return;
	else
		printTree( root );
}

template <class Comparable>
void AvlTree<Comparable>::
	printTree( AvlNode<Comparable> * t ) const
{
	if ( t != NULL )
	{
		printTree( t->left );
		cout << t->element << endl;
		printTree( t->right );
	}
}

template <class Comparable>
void AvlTree<Comparable>::printTreeHeight( ) const
{
	if( isEmpty( ) )
		cout << "Empty tree" << endl;
	else
		printTreeHeight( root );
}

template <class Comparable>
void AvlTree<Comparable>::
	printTreeHeight( AvlNode<Comparable> * t ) const
{
	if ( t != NULL )
	{
		printTreeHeight( t->left );
		cout << t->element <<": "<<t->height<< endl;
		printTreeHeight( t->right );
	}
}

template <class Comparable>
const AvlTree<Comparable> &  AvlTree<Comparable>::operator=( const AvlTree<Comparable> & rhs ){
	if (this != &rhs){
		makeEmpty();
		root = rhs.clone(rhs.root);
	}
	return *this;
}

template <class Comparable>
AvlNode<Comparable> *
	AvlTree<Comparable>::
	clone( AvlNode<Comparable> * t ) const
{
	if ( t == NULL )
		return NULL;
	else
		return new AvlNode<Comparable> 
		( t->element, 
		clone( t->left ), 
		clone( t->right ) );
}

template <class Comparable>
AvlTree<Comparable>::~AvlTree()
{
	makeEmpty( );
}

template <class Comparable>
AvlTree<Comparable>::AvlTree( const AvlTree<Comparable> & rhs ) : root( NULL ), ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND )
{ 
	*this = rhs;
}


//Item class
class Item{
public:
	Item(string t, string i=""): title(t), information(i){};
	string title;
	string information;


};
//ostream operator
ostream & operator << (ostream & os, const Item & item)   
{
	os << item.title;
	return os;
}
//make it comparable
bool operator < (const Item &  lhs,const Item &  rhs){
	return lhs.title < rhs.title;
}
bool operator > (const Item &  lhs,const Item &  rhs){
	return lhs.title > rhs.title;
}
bool operator == (const Item &  lhs,const Item &  rhs){
	return lhs.title == rhs.title;
}
bool operator != (const Item &  lhs,const Item &  rhs){
	return lhs.title != rhs.title;
}


//AVL section class
class secAVL{
public:
	secAVL(string t) : title(t), items(AvlTree<Item>(Item("NOT","NOT"))) {}
	string title;
	AvlTree<Item> items;
};
//ostream operator
ostream & operator << (ostream & os, const secAVL & sec)   
{
	os << sec.title;
	return os;
}
//make it comparable
bool operator < (const secAVL & lhs,const secAVL & rhs){
	return lhs.title < rhs.title;
}
bool operator > (const secAVL & lhs,const secAVL & rhs){
	return lhs.title > rhs.title;
}
bool operator == (const secAVL & lhs,const secAVL & rhs){
	return lhs.title == rhs.title;
}
bool operator != (const secAVL & lhs,const secAVL & rhs){
	return lhs.title != rhs.title;
}


//BST section class
class secBST{
public:
	secBST(string t) : title(t), items(BinarySearchTree<Item>(Item("NOT","NOT"))) {}
	string title;
	BinarySearchTree<Item> items;

};
//ostream operator
ostream & operator << (ostream & os, const secBST & sec)   
{
	os << sec.title;
	return os;
}
//make it comparable
bool operator < (const secBST & lhs,const secBST & rhs){
	return lhs.title < rhs.title;
}
bool operator > (const secBST & lhs,const secBST & rhs){
	return lhs.title > rhs.title;
}
bool operator == (const secBST & lhs,const secBST & rhs){
	return lhs.title == rhs.title;
}
bool operator != (const secBST & lhs,const secBST & rhs){
	return lhs.title != rhs.title;
}


//Menus and Their Functions

void printMainMenu(){
	cout<<"MENU"<<endl;
	cout<<"Please enter an input between [1 - 6]:"<<endl;
	cout<<"1- Display the sections [AVL]"<<endl<<
		"2- Display the sections [BST]"<<endl<<
		"3- Select a section"<<endl<<
		"4- Add new section"<<endl<<
		"5- Delete a section"<<endl<<
		"6- Exit"<<endl;

}

void displaysecAVL(const AvlTree<secAVL> & n){
	cout<<endl;
	cout<<"*****"<<endl;
	n.printTree();
	cout<<"*****"<<endl;
	cout<<endl;
}

void displaysecBST(const BinarySearchTree<secBST> & n){
	cout<<endl;
	cout<<"*****"<<endl;
	n.printTree();
	cout<<"*****"<<endl;
	cout<<endl;
}

void printSecMenu(){
	cout<<"Please enter an input between [1 - 7]:"<<endl;
	cout<<"1- Display the items [AVL]"<<endl<<
		"2- Display the items [BST]"<<endl<<
		"3- Display the information of a item"<<endl<<
		"4- Add new item"<<endl<<
		"5- Update the information of a item"<<endl<<
		"6- Delete an item"<<endl<<
		"7- Return to main menu"<<endl;
}

void displayitemAVL(AvlNode<secAVL>* n){
	cout<<endl;
	cout<<"*****"<<endl;
	n->element.items.printTree();
	cout<<"*****"<<endl;
	cout<<endl;
}

void displayitemBST(BinaryNode<secBST>* n){
	cout<<endl;
	cout<<"*****"<<endl;
	n->element.items.printTree();
	cout<<"*****"<<endl;
	cout<<endl;
}

void displayInfo(AvlNode<secAVL>* n1,BinaryNode<secBST>* n2 ){
	string input;
	long long bsttimer = 0;
	long long avltimer = 0;
	cout<<"Enter the title of the item: ";
	getline(cin, input);
	if(n1->element.items.find(Item(input)) == n1->element.items.ITEM_NOT_FOUND){

		auto start = std::chrono::high_resolution_clock::now();
		n1->element.items.find(Item(input));
		auto end = std::chrono::high_resolution_clock::now();
		avltimer =(end-start).count()/1000.0;
		start = std::chrono::high_resolution_clock::now();
		n2->element.items.find(Item(input));
		end = std::chrono::high_resolution_clock::now();
		bsttimer =(end-start).count()/1000.0;
		cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
		cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
		cout<<"Invalid title!"<<endl;
		cout<<endl;
		return;
	}
	string info;
	auto start = std::chrono::high_resolution_clock::now();
	info = n1->element.items.find(Item(input)).information;
	auto end = std::chrono::high_resolution_clock::now();
	avltimer =  (end-start).count()/1000.0;
	start = std::chrono::high_resolution_clock::now();
	info = n2->element.items.find(Item(input)).information;
	end = std::chrono::high_resolution_clock::now();
	bsttimer =(end-start).count()/1000.0;
	cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
	cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
	cout<<info<<endl;
	cout<<endl;

}

void additem(AvlNode<secAVL>* n1,BinaryNode<secBST>* n2){
	string input;
	string desc;
	long long bsttimer = 0;
	long long avltimer = 0;
	cout<<"Enter a title for the item: ";
	getline(cin, input);
	if(n1->element.items.find(Item(input)) != n1->element.items.ITEM_NOT_FOUND){

		cout<<"Item  \""<<input<<"\" already exists in the \""<<n1->element.title<<"\"."<<endl;
		cout<<endl;
		return;
	}
	cout<<"Enter a description for the item: ";
	getline(cin, desc);
	auto start = std::chrono::high_resolution_clock::now();
	n1->element.items.insert(Item(input,desc));
	auto end = std::chrono::high_resolution_clock::now();
	avltimer =(end-start).count()/1000.0;

	start = std::chrono::high_resolution_clock::now();
	n2->element.items.insert(Item(input,desc));
	end = std::chrono::high_resolution_clock::now();
	bsttimer =(end-start).count()/1000.0;

	cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
	cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
	cout<<"The new item \""<<input<<"\"has been inserted."<<endl;
	cout<<endl;


}

void updateInfo(AvlNode<secAVL>* n1,BinaryNode<secBST>* n2){
	string input;
	string desc;
	long long bsttimer = 0;
	long long avltimer = 0;
	cout<<"Enter the title of the item: ";
	getline(cin, input);
	if(n1->element.items.find(Item(input)) == n1->element.items.ITEM_NOT_FOUND){

		auto start = std::chrono::high_resolution_clock::now();
		n1->element.items.find(Item(input));
		auto end = std::chrono::high_resolution_clock::now();
		avltimer = (end-start).count()/1000.0;
		start = std::chrono::high_resolution_clock::now();
		n2->element.items.find(Item(input));
		end = std::chrono::high_resolution_clock::now();
		bsttimer = (end-start).count()/1000.0;
		cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
		cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
		cout<<"Item \""<<input<<"\" does not exist in the \""<<n1->element.title<<"\"."<<endl;
		cout<<endl;
		return;
	}
	cout<<"Enter the new information: ";
	getline(cin, desc);
	auto start = std::chrono::high_resolution_clock::now();
	n1->element.items.find(Item(input),n1->element.items.root)->element.information = desc;
	auto end = std::chrono::high_resolution_clock::now();
	avltimer =(end-start).count()/1000.0;
	start = std::chrono::high_resolution_clock::now();
	n2->element.items.find(Item(input),n2->element.items.root)->element.information = desc;
	end = std::chrono::high_resolution_clock::now();
	bsttimer =(end-start).count()/1000.0;
	cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
	cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
	cout<<"The content "<<input<<" has updated."<<endl;
	cout<<endl;

}

void delitem(AvlNode<secAVL>* n1,BinaryNode<secBST>* n2){
	string input;

	long long bsttimer = 0;
	long long avltimer = 0;
	cout<<"Enter a title for the item: ";
	getline(cin, input);
	if(n1->element.items.find(Item(input)) == n1->element.items.ITEM_NOT_FOUND){
		cout<<"Item  \""<<input<<"\" does not exist in the \""<<n1->element.title<<"\"."<<endl;
		cout<<endl;
		return;
	}
	auto start = std::chrono::high_resolution_clock::now();
	n1->element.items.remove(Item(input));
	auto end = std::chrono::high_resolution_clock::now();
	avltimer =(end-start).count()/1000.0;

	start = std::chrono::high_resolution_clock::now();
	n2->element.items.remove(Item(input));
	end = std::chrono::high_resolution_clock::now();
	bsttimer =(end-start).count()/1000.0;
	cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
	cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
	cout<<"The item \""<<input<<"\"has been deleted."<<endl;
	cout<<endl;
}

void processSecMenu(AvlNode<secAVL>* n1,BinaryNode<secBST>* n2 ) { // add parameters to inner functions which is reachable from this function's parameters
	char input;
	string dummy;
	printSecMenu();
	do{
		cout << "Input: " ;
		cin >> input;
		getline(cin,dummy);
		while(dummy.length() != 0){
			cout << "Invalid option: please enter again" << endl;
			cin >> input;
			getline(cin,dummy);
		}
		switch (input) {
		case '7':
			cout<<endl;
			printMainMenu();
			return;
		case '1':
			displayitemAVL(n1);
			break;
		case '2':
			displayitemBST(n2);
			break;
		case '3':
			displayInfo(n1,n2);
			break;
		case '4':
			additem(n1,n2);
			break;
		case '5':
			updateInfo(n1,n2);
			break;
		case '6':
			delitem(n1,n2);
			break;
		default:
			cout << "Invalid option: please enter again" << endl;
		}
	} while(true);
}

void selectsec( AvlTree<secAVL> & n1,BinarySearchTree<secBST> & n2){
	string input;
	cout<<"Enter the title of the section: ";
	getline(cin, input);
	if(n1.find(secAVL(input)) == n1.ITEM_NOT_FOUND){
		cout<<"Invalid title!"<<endl;
		cout<<endl;
		return;
	}
	cout<<endl;
	cout<<"Selected section -> "<<input<<endl;

	processSecMenu(n1.find(secAVL(input),n1.root),n2.find(secBST(input),n2.root));
}

void addsec( AvlTree<secAVL> & n1,BinarySearchTree<secBST> & n2){
	string input;
	cout<<"Enter the title of the section: ";
	getline(cin, input);
	if(n1.find(secAVL(input)) != n1.ITEM_NOT_FOUND){
		cout<<"Section \""<<input<<"\" already exists."<<endl;
		cout<<endl;
		return;

	}
	n1.insert(secAVL(input));
	n2.insert(secBST(input));
	cout<<"The new section \""<<input<<"\" has been inserted."<<endl;
	cout<<endl;
}

void delsec( AvlTree<secAVL> & n1,BinarySearchTree<secBST> & n2){
	string input;
	cout<<"Enter the title of the section: ";
	getline(cin, input);
	if(n1.find(secAVL(input)) == n1.ITEM_NOT_FOUND){
		cout<<"Section \""<<input<<"\" does not exist."<<endl;
		cout<<endl;
		return;
	}
	n1.remove(n1.find(secAVL(input),n1.root)->element);
	n2.remove(n2.find(secBST(input),n2.root)->element);
	cout<<"The section has been deleted."<<endl;
	cout<<endl;
}

void processMainMenu(AvlTree<secAVL> & n1,BinarySearchTree<secBST> & n2) { // add parameters to inner functions which is reachable from this function's parameters
	char input;
	string dummy;
	printMainMenu();
	do{
		cout << "Input: ";
		cin >> input;
		getline(cin,dummy);
		while(dummy.length() != 0){
			cout << "Invalid option: please enter again" << endl;
			cin >> input;
			getline(cin,dummy);
		}
		switch (input) {
		case '6':
			cout<<endl;
			cout << "Terminating..." << endl;
			return;
		case '1':
			displaysecAVL(n1);
			break;
		case '2':
			displaysecBST(n2);
			break;
		case '3':
			selectsec( n1,n2);
			break;
		case '4':
			addsec(n1,n2);
			break;
		case '5':
			delsec(n1,n2);
			break;

		default:
			cout << "Invalid option: please enter again" << endl;
		}
	} while(true);
}



int main(){


	// AVL Tester with integers
	/*AvlTree<int> t(-1);
	t.insert(8);
	t.insert(3);
	t.insert(10);
	t.insert(2);
	t.insert(12);
	t.insert(5);
	t.insert(4);
	t.insert(6);
	t.printTree();
	cout<<endl;
	t.printTreeHeight();
	t.remove(12);
	cout<<endl;
	t.printTree();
	cout<<endl;
	t.printTreeHeight();
	t.insert(14);
	t.insert(12);
	t.insert(6);
	t.insert(2);
	t.insert(5);
	t.insert(10);
	t.insert(1);
	t.insert(4);
	t.insert(7);
	t.insert(3);
	t.insert(13);
	t.printTree();
	cout<<endl;
	t.printTreeHeight();
	t.remove(10);
	cout<<endl;
	t.printTree();
	cout<<endl;
	t.printTreeHeight();
	cout<<endl;
	*/
	//--------------------------------------------------------

	//Program Starts

	//creating the notebooks AVL and BST
	BinarySearchTree<secBST> noteBST(secBST("NOT"));
	AvlTree<secAVL> noteAVL(secAVL("NOT"));

	//reading process STARTS
	ifstream input;          		
	string filename = "data.txt";
	string s;

	cout<<"Welcome to the Notes"<<endl;
	cout<<endl;
	input.open(filename.c_str());


	string currSec;
	long long bsttimer = 0;
	long long avltimer = 0;
	bool enter = false;
	while(getline(input,s)){
		if(s[0] != '-'){
			if(enter){
				cout<<"Section \""<<currSec<<"\" has been inserted into the AVL notebook."<<endl;
				cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
				cout<<"Section \""<<currSec<<"\" has been inserted into the BST notebook."<<endl;
				cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
				cout<<endl;
				bsttimer = 0;
				avltimer = 0;
			}
			//section 
			currSec = s;
			//section insert bst
			auto start = std::chrono::high_resolution_clock::now();
			noteBST.insert(secBST(s));
			auto end = std::chrono::high_resolution_clock::now();
			bsttimer += (end-start).count()/1000.0;
			//section insert avl
			start = std::chrono::high_resolution_clock::now();
			noteAVL.insert(secAVL(s));
			end = std::chrono::high_resolution_clock::now();
			avltimer += (end-start).count()/1000.0;

			enter = true;

		}
		else{
			string title = s.substr(1,s.rfind('-')-1);
			string info = s.substr(s.rfind('-')+1);
			Item item(title,info);

			//item insert bst
			auto start = std::chrono::high_resolution_clock::now();
			noteBST.find(secBST(currSec),noteBST.root)->element.items.insert(item);
			auto end = std::chrono::high_resolution_clock::now();
			bsttimer +=(end-start).count()/1000.0;

			//item insert avl
			start = std::chrono::high_resolution_clock::now();
			noteAVL.find(secAVL(currSec),noteAVL.root)->element.items.insert(item);
			end = std::chrono::high_resolution_clock::now();
			avltimer += (end-start).count()/1000.0;
		}

	}
	if(enter){
		cout<<"Section \""<<currSec<<"\" has been inserted into the AVL notebook."<<endl;
		cout<<"[AVL] Elapsed time: "<<avltimer<<" microseconds"<<endl;
		cout<<"Section \""<<currSec<<"\" has been inserted into the BST notebook."<<endl;
		cout<<"[BST] Elapsed time: "<<bsttimer<<" microseconds"<<endl;
		bsttimer = 0;
		avltimer = 0;
		cout<<endl;
	}
	input.close();

	//reading process ENDS


	//Menu 

	processMainMenu(noteAVL,noteBST);
	
	return 0;
}

