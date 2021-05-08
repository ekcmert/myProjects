#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

//CS300 HW3 - Personalized Recommendation System for TV Show Streaming Platforms
//Mert Ekici 26772 ekicimert@sabanciuniv.edu



//LINKED LIST implementation for separate chaining !!!
template <class Object>
class List;     // Incomplete declaration.

template <class Object>
class ListItr;     // Incomplete declaration.

template <class Object>
class ListNode
{
	ListNode( const Object & theElement = Object( ), ListNode * n = NULL )
		: element( theElement ), next( n ) { }

	Object   element;
	ListNode *next;

	friend class List<Object>;
	friend class ListItr<Object>;
	
};
 
template <class Object>
class ListItr
{
public:
	ListItr( ) : current( NULL ) { }
	bool isPastEnd( ) const
	{   return current == NULL;   }
	void advance( )
	{   if ( ! isPastEnd( ) )   current = current->next;   }
	const Object & retrieve( ) const
	{   if ( isPastEnd( ) )    cout<<"pastend"<<endl;
	return current->element;    }

private:
	ListNode<Object> *current;    // Current position

	ListItr( ListNode<Object> *theNode )
		: current( theNode ) { }

	friend class List<Object>;    // Grant access to constructor
};

template <class Object>
class List
{
public:
	List( );
	List( const List & rhs );
	~List( );
	bool isEmpty( ) const;
	void makeEmpty( );
	ListItr<Object> zeroth( ) const;
	ListItr<Object> first( ) const;
	void insert( const Object & x, const ListItr<Object> & p );
	ListItr<Object> find( const Object & x ) const;
	ListItr<Object> findPrevious( const Object & x ) const;
	void remove( const Object & x );

	const List & operator=( const List & rhs );

private:
	ListNode<Object> *header;
};



//Construct the list.
template <class Object>
List<Object>::List( )
{
	header = new ListNode<Object>;
}

//Test if the list is logically empty. Return true if empty, false, otherwise.
template <class Object>
bool List<Object>::isEmpty( ) const
{    
	// see if the header points to NULL 
	return header->next == NULL;
}

//Insert item x after p.
template <class Object>
void List<Object>::insert( const Object & x, const ListItr<Object> & p )
{
	if ( p.current != NULL )
		p.current->next = new ListNode<Object>( x, p.current->next );
}

//Return iterator corresponding to the first node containing an item x. Iterator isPastEnd if item is not found.
template <class Object>
ListItr<Object> List<Object>::find( const Object & x ) const
{
	ListNode<Object> *itr = header->next; // Initialize

	while ( itr != NULL && itr->element != x )
		itr = itr->next;

	return ListItr<Object>( itr );
}

//Return iterator prior to the first node containing an item x. */
template <class Object>
ListItr<Object> List<Object>::findPrevious( const Object & x ) const
{
	ListNode<Object> *itr = header;

	while ( itr->next != NULL && itr->next->element != x )
		itr = itr->next;

	return ListItr<Object>( itr );
}

//Remove the first occurrence of an item x.
template <class Object>
void List<Object>::remove( const Object & x )
{
	ListItr<Object> p = findPrevious( x ); // Locate previous of x, if any

	if ( p.current->next != NULL )
	{
		ListNode<Object> *oldNode = p.current->next;
		p.current->next = p.current->next->next;  // Bypass deleted node
		delete oldNode;
	}
}

//Return an iterator representing the header node.
template <class Object>
ListItr<Object> List<Object>::zeroth( ) const
{
	return ListItr<Object>( header );
}

//Return an iterator representing the first node in the list. This operation is invalid for empty lists.
template <class Object>
ListItr<Object> List<Object>::first( ) const
{
	return ListItr<Object>( header->next );
}

//Make the list logically empty.
template <class Object>
void List<Object>::makeEmpty( )
{
	while ( ! isEmpty( ) )
		remove( first( ).retrieve( ) );
}

//Deep copy of linked lists.
template <class Object>
const List<Object> & List<Object>::operator=( const List<Object> & rhs )
{
	if ( this != &rhs )
	{
		makeEmpty( );

		ListItr<Object> ritr = rhs.first( );
		ListItr<Object> itr = zeroth( );
		for (  ;  ! ritr.isPastEnd( ); ritr.advance( ), itr.advance( ) )
			insert( ritr.retrieve( ), itr );
	}
	return *this;
}

//Copy constructor.
template <class Object>
List<Object>::List( const List<Object> & rhs )
{
	header = new ListNode<Object>;
	*this = rhs;  // Deep Copy
}

//Destructor.
template <class Object>
List<Object>::~List( )
{
	makeEmpty( );  // Get rid of all list nodes
	delete header;  // then get rid of the header
}

template <class Object>
void printList( const List<Object> & theList )
{
	if ( theList.isEmpty( ) )
		;
	else
	{
		ListItr<Object> itr = theList.first( );
		for ( ; ! itr.isPastEnd( ); itr.advance( ) )
			cout << itr.retrieve( ) << " ";
		cout << endl;
	}
	
}


//Implementation of the templated HASH TABLE with separate chaining !!!
template <class HashedObj>
class HashTable
{
public:
	HashTable( const HashedObj & notFound, 
		int size = 101 );
	HashTable( const HashTable & rhs )
		: ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND ), 
		theLists( rhs.theLists ) { }

	const HashedObj & find( const HashedObj & x ) const;

	void makeEmpty( );
	void insert( const HashedObj & x );
	void remove( const HashedObj & x );
	void print();
	vector<HashedObj> convert();
	

	const HashTable & operator=( const HashTable & rhs);
private:
	vector<List<HashedObj> > theLists;   
	// The array of Lists
	const HashedObj ITEM_NOT_FOUND;
};

int hash( const string & key, int tableSize );
int hash( int key, int tableSize );

//Construct the hash table.
template <class HashedObj>
HashTable<HashedObj>::HashTable( 
	const HashedObj & notFound, int size )
	: ITEM_NOT_FOUND( notFound ), theLists( nextPrime( size ) )
{}

//Internal method to test if a positive number is prime. Not an efficient algorithm.
bool isPrime( int n )
{
	if ( n == 2 || n == 3 )
		return true;

	if ( n == 1 || n % 2 == 0 )
		return false;

	for ( int i = 3; i * i <= n; i += 2 )
		if ( n % i == 0 )
			return false;

	return true;
}

//Internal method to return a prime number at least as large as n.  Assumes n > 0.
int nextPrime( int n )
{
	if ( n % 2 == 0 )
		n++;

	for ( ; ! isPrime( n ); n += 2 )
		;

	return n;
}

//Make the hash table logically empty.
template <class HashedObj>
void HashTable<HashedObj>::makeEmpty( )
{
	for( int i = 0; i < theLists.size( ); i++ )
		theLists[ i ].makeEmpty( );  
	// destroy the lists but not the vector!
}

//Insert item x into the hash table. If the item is already present, then do nothing.
template <class HashedObj>
void HashTable<HashedObj>::insert( const HashedObj & x )
{
	// hash the given object and locate the list it should be on 
	List<HashedObj> & whichList = theLists[ ::hash( x, theLists.size( ) ) ];
	// locate the object in the list (using List’s find)
	ListItr<HashedObj> itr = whichList.find( x );
	// insert the new item at the head of the list if not found!
	if ( itr.isPastEnd( ) )
		whichList.insert( x, whichList.zeroth( ) );
}

//Remove item x from the hash table.
template <class HashedObj>
void HashTable<HashedObj>::remove( const HashedObj & x )
{
	// remove from the appropriate list
	theLists[ hash( x, theLists.size( ) ) ].remove( x );
}

//Find item x in the hash table.Return the matching item or ITEM_NOT_FOUND if not found
template <class HashedObj>
const HashedObj & HashTable<HashedObj>::
	find( const HashedObj & x ) const
{
	ListItr<HashedObj> itr;
	// locate the approriate list and search there
	itr = theLists[ ::hash( x, theLists.size( ) ) ].find( x );
	// retrieve from the located position
	if ( itr.isPastEnd( ) )
		return ITEM_NOT_FOUND;

	return itr.retrieve( );
}

template <class HashedObj>
void HashTable<HashedObj>::
	print(){
		for(int i=0; i< theLists.size();i++){
			printList(theLists[i]);
		}
}

template <class HashedObj>
vector<HashedObj> HashTable<HashedObj>::
	convert(){

		vector<itemSet> vec;

		for(int i=0; i<theLists.size();i++){
			List<HashedObj> & whichList = theLists[i];
			ListItr<HashedObj> itr = whichList.first();
			while(!itr.isPastEnd() ){
				vec.push_back(itr.retrieve());
				itr.advance();
			}
		}

		return vec;

}

//hash functions for int and string specific
int hash (const int key, const int tableSize)
{ 
	  return  (key % tableSize);
}
int hash (const string & key, int tableSize)
{ 
      int sum = 0;
      for (int i = 0; i < key.length(); i++) // add all bytes in a loop
           sum = sum + key[ i ];
      return (sum % tableSize);
}


//itemSet class

class itemSet{
public:
	
	void push_back(string s){
		for(int i=0; i<set.size(); i++){
			if(set[i]==s){
				return;
			}
		}
		set.push_back(s);
	}

	void pop_back(){set.pop_back();}

	

	

	vector<string> set;
	float support;

};

//== != operators for maintaining the set property
bool operator == (const itemSet& lhs,const itemSet & rhs){
		if(lhs.set.size()!=rhs.set.size()){
			return false;
		}
		else{
			for(int i=0; i<lhs.set.size(); i++){
				bool lap=false;
				for(int k=0; k<lhs.set.size(); k++){
					if(lhs.set[i] == rhs.set[k]){
						lap=true;
					}
				}
				if(!lap){
					return lap;
				}
			}
			return true;
		}
	}
bool operator != (const itemSet& lhs,const itemSet & rhs){
	return !(lhs==rhs);
}

//ostream for itemSet class
ostream & operator << (ostream & os, const itemSet & set)
{
	for(int i=0;i<set.set.size();i++){
		if(i>0)
			os<<","<<set.set[i];
		else
			os<<set.set[i];
	}
	//os<<": "<<set.support;
	return  os;
}

//merge two itemSets (for calculating rule's confidence)
itemSet merge (const itemSet& s1,const itemSet& s2){
	itemSet res;

	for(int i=0; i<s1.set.size();i++){
		res.push_back(s1.set[i]);
	}
	for(int i=0; i<s2.set.size();i++){
		res.push_back(s2.set[i]);
	}

	return res;
} 

//itemSet hash function (similar to string hash function)
int hash (const itemSet & set, int tableSize){
	int sum = 0;
	for(int k = 0; k<set.set.size();k++){
		for (int i = 0; i < set.set[k].length(); i++) // add all bytes in a loop
			sum = sum + set.set[k][i];}
	return (sum % tableSize);
}


//rule class

class rule{
public:

	rule(const itemSet & f,const itemSet & s){
		association.first=f;
		association.second=s;
	}

	bool operator ==(rule rhs) {
		return (rhs.association.first == association.first) && (rhs.association.second == association.second);
	}


	pair<itemSet,itemSet> association;
	float conf;
};

//rule ostream operator
ostream & operator << (ostream & os, const rule & r)
{
	os<<r.association.first<<";"<<r.association.second<<";"<<r.conf;
	return  os;
}



//matrix printer
void printMatrix(vector<vector<string> > & v){
	for(int i=0; i<v.size(); i++){
		for(int k=0; k<v[i].size(); k++){
			cout<<v[i][k]<<" ";
		}
		cout<<endl;
	}
}

//write rules to a file
void writeVector(string filename, const vector<rule>& rules){
	ofstream file (filename);
	for(int i=0; i<rules.size(); i++){
		file<<rules[i]<<endl;
	}
	
}

//vector printer
template <class obj>
void printVector(vector<obj> & v){
	for(int i=0; i<v.size(); i++){
		cout<<v[i]<<endl;
	}
}

//extract shows from the file
itemSet extractShows(string filename){
	
	itemSet shows;
	ifstream input;
	string s;

	input.open(filename.c_str());
	while(getline(input,s)){
		string showName;
		istringstream istr(s);
		vector<string> line;
		while(istr>>showName){
			shows.push_back(showName);
		}
	}
	input.close();

	return shows;

	
}

//support calculation
float support(string filename, const itemSet & set){

	ifstream input;
	string s;
	

	input.open(filename.c_str());
	float denominator = 0;
	float numerator = 0;
	while(getline(input,s)){
		denominator++;
		
		
		string showName;
		istringstream istr(s);
		vector<string> line;
		int counter = 0;
		while(istr>>showName){
			for(int i=0;i<set.set.size();i++){
				if(set.set[i] == showName) counter++;
			}
		}
		if(counter == set.set.size()){
			numerator++;
		}
	
	}
	input.close();
	return numerator/denominator;
}

//confidence calculation (using support function)
float confidence(string filename, const rule & r, const HashTable<itemSet>& table){
	float numerator;
	float denominator;

	itemSet ITEM_NOT_FOUND;
	ITEM_NOT_FOUND.push_back("ITEM_NOT_FOUND");

	if(table.find(r.association.first) == ITEM_NOT_FOUND)
		denominator = support(filename,r.association.first);
	else
		denominator = table.find(r.association.first).support;

	itemSet mergedset = merge(r.association.first,r.association.second);

	numerator = support(filename,mergedset);
	return numerator/denominator;
}

// chech if there are any common elements of these itemSets (then not use them as a rule)
template<class obj>
bool isCom(const vector<obj> & a, const vector<obj> & b){
	for(int i= 0; i<a.size(); i++){
		for(int k=0; k<b.size(); k++){
			if(a[i] == b[k]){
				return true;
			}
		}
	}
	return false;
}

//round function (2 decimals)
float round(float num)
{

    float value = (int)(num * 100 + .5);
    return (float)value / 100;
}

int main(){

	float sup;
	float con;
	string filename;

	//input session
	cout<<"Please enter the transaction file name: ";
	cin>>filename;

	cout<<"Please enter support and confidence values between 0 and 1: ";
	cin>>sup>>con;

	while(sup<0||sup>1||con<0||con>1){
		cout<<"Please enter values between 0 and 1: ";
		cin>>sup>>con;
	}
	//---------


	itemSet ITEM_NOT_FOUND;
	ITEM_NOT_FOUND.push_back("ITEM_NOT_FOUND");
	HashTable<itemSet> lookupTable(ITEM_NOT_FOUND);
	vector<itemSet> selectedsets;

	//old file method
	/*ifstream input;
	string s;
	input.open(filename.c_str());
	while(getline(input,s)){
		string showName;
		istringstream istr(s);
		vector<string> line;
		while(istr>>showName){
			line.push_back(showName);
		}
		data.push_back(line);
	}
	input.close();
	printMatrix(shows.set);*/

	itemSet shows = extractShows(filename);
	itemSet selectedshows;
	
	//add shows to lookupTable (by support values >= threshold)
	for(int i=0; i<shows.set.size(); i++){
		itemSet dum;
		dum.push_back(shows.set[i]); 
		dum.support = support(filename,dum);
		if(dum.support >= sup){
	//	cout<<support(filename,dum)<<" "<<shows.set[i]<<endl;  //test
		lookupTable.insert(dum);
		selectedshows.push_back(shows.set[i]);}
	}

	//using selectedshows set, add pairs to lookupTable (support >= threshold)
	for(int i=0; i<selectedshows.set.size(); i++){
		for(int k=0; k<selectedshows.set.size(); k++){
			if(selectedshows.set[i]!=selectedshows.set[k]){
				itemSet nset;
				nset.push_back(selectedshows.set[i]);
				nset.push_back(selectedshows.set[k]);
				nset.support=support(filename,nset);
				if(nset.support>= sup){
					lookupTable.insert(nset);
				}
			}
		}
	}


	//extract lookupTable elements to selectedsets to find paired permutations for rules
	selectedsets = lookupTable.convert();
	vector<rule> rules;


	//find possible rules with confidence >= threshold
	for(int i = 0; i<selectedsets.size(); i++){
		for(int k =0; k<selectedsets.size(); k++){
			if(selectedsets[k] != selectedsets[i] && !isCom(selectedsets[k].set,selectedsets[i].set)){
				rule nrule(selectedsets[i],selectedsets[k]);
				bool notyet = false;
				for(int j=0; j<rules.size();j++){
					if(nrule == rules[j]){
						notyet = true;
					}
				}
				if(!notyet){
					nrule.conf= round(confidence(filename,nrule,lookupTable));
					if(nrule.conf>=con)
						rules.push_back(nrule);
				}
			}
		}
	}
	
	//console output
	if(rules.size()>0){
		if(rules.size()>1){
			cout<<rules.size()<<" rules are written to results.txt"<<endl;
		}
		else if(rules.size()==1){
			cout<<rules.size()<<" rule is written to results.txt"<<endl;
		}
		writeVector("results.txt",rules);
	}
	else{
		cout<<"There is no rule for the given support and confidence values."<<endl;
		writeVector("results.txt",rules);
	}


	//find tests with different order
	/*itemSet show;
	show.push_back("friends");
	show.push_back("seinfeld");
	cout<<lookupTable.find(show);
	cout<<endl;
	*/

	//print tests
	/*
	lookupTable.print();
	cout<<endl<<endl;
	printVector(rules);
	writeMatrix("result.txt",rules);*/

	//lookup,selected comparison test
	/*cout<<endl;
	printMatrix(selectedsets);
	cout<<endl;
	printMatrix(lookupTable.convert());*/
	
	//hash find test
	/*
	itemSet show;
	show.push_back("friends");
	itemSet show1;
	show1.push_back("friends1");

	cout<<lookupTable.find(show).set[0];
	cout<<lookupTable.find(show1).set[0];
	*/

	//Class Tests
	/*
	itemSet a;
	itemSet b;
	itemSet c;

	c.set.push_back("final");

	a.set.push_back("ali");a.set.push_back("veli");a.set.push_back("lo");a.set.push_back("mero");a.set.push_back("tero");
	b.set.push_back("veli");b.set.push_back("mero");b.set.push_back("tero");b.set.push_back("lo");b.set.push_back("ali");

	cout<<(a==b)<<endl;

	rule ar1(a,c);
	rule br1(b,c);

	rule ar2(c,a);
	rule br2(c,c);

	rule ar3(a,a);
	rule br3(b,b);


	cout<<((ar1==br1)&&(ar2==br2)&&(ar3==br3))<<endl; */

return 0;
}