#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
using namespace std;

//Mert Ekici 26772 
//CS300 HW1 (Knight to the Rescue)

//Struct for board cells
struct Cell{
	int x_coordinate;
	int y_coordinate;
	bool visited;
	bool has_pawn;
	bool has_king;

	//unavailable cells for the king rescue
	bool unavailable;

	//unavailable cells vector for each cell (pawn placement)
	vector<Cell> unavailables;

	//constructor
	Cell(int x=-1, int y=-1):x_coordinate(x), y_coordinate(y),visited(false),has_pawn(false),has_king(false),unavailable(false){}
};


//stack class from the lectures (base class will be inherited as chessStack)
template <class Object>
class Stack{
public:
	Stack( ){
		topOfStack = NULL;
	}

	Stack( const Stack & rhs ){
		topOfStack = NULL;
		*this = rhs; // deep copy 
	}

	~Stack( ){
		makeEmpty( );
	}

	bool isEmpty( ) const {
		return topOfStack == NULL;
	}

	void makeEmpty( ){
		while ( ! isEmpty( ) )
			pop( );
	}

	void pop( ){
		if ( isEmpty( ) )
			cout<<"Stack is empty!"<<endl;
		ListNode *oldTop = topOfStack;
		topOfStack = topOfStack->next;
		delete oldTop;
	}

	void push( const Object & x ){
		topOfStack = new ListNode(x, topOfStack);
	}

	Object topAndPop( ){
		Object topItem = top( );
		pop( );
		return topItem;
	}

	const Object & top( ) const{
		if ( isEmpty( ) )
			cout<<"Stack is empty!"<<endl;
		return topOfStack->element;
	}


	const Stack & operator=( const Stack & rhs ) {
		if ( this != &rhs )
		{
			makeEmpty( );
			if ( rhs.isEmpty( ) )
				return *this;

			ListNode *rptr = rhs.topOfStack;
			ListNode *ptr  = new ListNode( rptr->element );
			topOfStack = ptr;

			for ( rptr = rptr->next; rptr != NULL; rptr = rptr->next )
				ptr = ptr->next = new ListNode( rptr->element );
		}
		return *this;
	}

	//protected for the use of subclasses
protected:
	struct ListNode
	{
		Object   element;
		ListNode *next;

		ListNode( const Object & theElement, ListNode * n = NULL )
			: element( theElement ), next( n ) { }


	};

	ListNode *topOfStack;  

};

class chessStack: public Stack<Cell>{
public:

	//non board stack constructor
	chessStack():Stack(), size(-1){}

	//board constructor
	chessStack(int n):Stack(){
		size = n;
		for(int y=1; y<n+1; y++){
			for(int x=1; x<n+1; x++){
				Cell ncell = Cell(x,y);
				push(ncell);
			}
		}
	}

	//returns the number of elements
	int numElements(){
		ListNode * curr = topOfStack;
		int counter = 0;
		while(curr){
			counter++;
			curr = curr->next;
		}
		return counter;

	}

	//random pop 
	Cell randPop( ){

		int randiter = rand() %numElements();
		for(int i = 0; i < randiter; i++){
			pop();
		}
		return topOfStack->element;
	}

	//print (differentiates for the board stack or linear chessStack)
	void Print() const{
		ListNode * curr = topOfStack;
		if(size != -1){
			for(int y=0; y<size; y++){
				Stack holder;
				for(int x=0; x<size; x++){
					holder.push(curr->element);
					curr = curr->next;
				}
				for(int x=0; x<size; x++){
					if(holder.topAndPop().has_pawn){
						cout<<"1 ";
					}
					else{
						cout<<"0 ";
					}
				}

				cout<<endl;
			}
		}
		else{
			while(curr){
				cout<<curr->element.x_coordinate<<" "<<curr->element.y_coordinate<<endl;
				curr = curr->next;
			}
		}
	}

	//reverse print function
	void reversePrint(){
		ListNode * curr = topOfStack;
		chessStack holder;
		while(curr){
			holder.push(curr->element);
			curr = curr->next;
		}
		holder.Print();
	}

	//reverses the stack
	void reverse(){
		ListNode * curr = topOfStack;
		chessStack newstack;
		while(curr){
			newstack.push(curr->element);
			curr = curr->next;
		}
		*this = newstack;
	}

	//finds the cell in this stack whose coordinates are given as parameters
	Cell & find(int x, int y) const{
		ListNode * curr = topOfStack;
		while(curr){
			if(curr->element.x_coordinate == x && curr->element.y_coordinate == y){
				return curr->element;
			}
			curr = curr->next;
		}
	}

	//checks the given cell is available for a pawn placement considering the constraints
	bool can_pawnCheck(const Cell & c) const{

		int x = c.x_coordinate;
		int y = c.y_coordinate;

		//diagonal count 1 = (/)
		int dia1count = 0;
		for(int i = -1; (x+i)>0 && (y+i)>0; i--){
			if( find(x+i,y+i).has_pawn ){
				dia1count++;
			}
		}
		for(int i = 1; (x+i)<size+1 && (y+i)<size+1; i++){
			if( find(x+i,y+i).has_pawn ){
				dia1count++;
			}
		}
		if(dia1count>=3){
			return false;
		}

		//diagonal count 2 = (\)
		int dia2count = 0;
		for(int i = -1; (x+i)>0 && (y-i)<size+1; i--){
			if( find(x+i,y-i).has_pawn ){
				dia2count++;
			}
		}
		for(int i = 1; (x+i)<size+1 && (y-i)>0; i++){
			if( find(x+i,y-i).has_pawn ){
				dia2count++;
			}
		}
		if(dia2count>=3){
			return false;
		}


		//vertical count
		int vercount = 0;
		for(int i = 1; (y+i)<size+1; i++){
			if( find(x,y+i).has_pawn ){
				vercount++;
			}
		}
		for(int i = 1; (y-i)>0; i++){
			if( find(x,y-i).has_pawn ){
				vercount++;
			}
		}
		if(vercount>=3){
			return false;
		}



		//horizontal count
		int horcount = 0;
		for(int i = 1; (x+i)<size+1; i++){
			if( find(x+i,y).has_pawn ){
				horcount++;
			}
		}
		for(int i = 1; (x-i)>0; i++){
			if( find(x-i,y).has_pawn ){
				horcount++;
			}
		}
		if(horcount>=3){
			return false;
		}
		return true;
	}


	chessStack& operator+=( const chessStack & rhs ) {
		chessStack r = rhs;
		if ( this != &rhs )
		{
			while(!r.isEmpty()){
				push(rhs.topOfStack->element);
				r.pop();
			}
		}

		return *this;
	}

	//counts pawns in the board
	int pawnCounter() const{
		ListNode * curr = topOfStack;
		int pawncount=0;
		while(curr){
			if(curr->element.has_pawn){
				pawncount++;
			}
			curr = curr->next;
		}
		return pawncount;
	}

	//resets unavailable cells as "false"
	void resetUnavail(){
		ListNode * curr = topOfStack;

		while(curr){
			curr->element.unavailable=false;
			curr = curr->next;
		}
	}


	friend chessStack pawnMoves(const Cell & pawn, const chessStack & board);

	//fills cells with appropriate unavailable values for the given condition of the board
	void fillUnavail(){
		ListNode * curr = topOfStack;

		while(curr){
			curr->element.unavailable=pawnMoves(curr->element,*this).isEmpty();
			curr = curr->next;
		}
	}

	//resets pawns (has_pawn = false)
	void resetPawns(){
		ListNode * curr = topOfStack;

		while(curr){
			curr->element.has_pawn=false;
			curr = curr->next;
		}
	}

	//resets visited as false
	void resetVisited(){
		ListNode * curr = topOfStack;

		while(curr){
			curr->element.visited=false;
			curr = curr->next;
		}
	}

	//counts unavailables (for debugging)
	int unavailableCounter() const{
		ListNode * curr = topOfStack;
		int pawncount=0;
		while(curr){
			if(curr->element.unavailable){
				pawncount++;
			}
			curr = curr->next;
		}
		return pawncount;
	}

	// output stream version of print function (writes the outputs in the given ostream)
	ostream& ostreamer(ostream & os){
		ListNode * curr = topOfStack;
		if(size != -1){
			for(int y=0; y<size; y++){
				Stack holder;
				for(int x=0; x<size; x++){
					holder.push(curr->element);
					curr = curr->next;
				}
				for(int x=0; x<size; x++){
					if(holder.topAndPop().has_pawn){
						os<<"1 ";
					}
					else{
						os<<"0 ";
					}
				}

				os<<endl;
			}
		}
		else{
			while(curr){
				os<<curr->element.x_coordinate<<" "<<curr->element.y_coordinate<<endl;
				curr = curr->next;
			}
		}
		return os;
	}

	// if -1 linear stack, else chessboard(if size = 8 that means 8x8 chessboard)
	int size;

};


//lists the possible moves of the given pawn in the given board in a chessStack
chessStack pawnMoves(const Cell & pawn, const chessStack & board){
	chessStack moves;
	int x = pawn.x_coordinate;
	int y = pawn.y_coordinate;
	int n = board.size;


	for(int dx =-1; dx < 2; dx++){
		for(int dy = -1; dy< 2; dy++){
			if((x+dx)>0 && (y+dy)>0 && (x+dx)<(n+1) && (y+dy)<(n+1) && board.can_pawnCheck(board.find(x+dx,y+dy)) && !board.find((x+dx),(y+dy)).has_pawn  && !((dx == 0) && (dy == 0))&& !board.find((x+dx),(y+dy)).unavailable ){
				moves.push(board.find(x+dx,y+dy));
			}
		}
	}


	return moves;
}

//make empty function for vectors
void vectorEmpty(vector<Cell> & vec){
	while(vec.size()!=0){
		vec.pop_back();
	}
}

//if finds given cell in the vector returns true (for unavailables vector)
bool vectorFind(const vector<Cell> & vec, const Cell& c){
	for (int i = 0; i<vec.size(); i++){
		if(vec[i].x_coordinate == c.x_coordinate && vec[i].y_coordinate == c.y_coordinate){
			return true;
		}
	}
	return false;
}

//checks unavailables cell specific (better version of pawnmoves)
chessStack pawnMovesVec(const Cell & pawn, const chessStack & board){
	chessStack moves;
	int x = pawn.x_coordinate;
	int y = pawn.y_coordinate;
	int n = board.size;


	for(int dx =-1; dx < 2; dx++){
		for(int dy = -1; dy< 2; dy++){
			if((x+dx)>0 && (y+dy)>0 && (x+dx)<(n+1) && (y+dy)<(n+1) && board.can_pawnCheck(board.find(x+dx,y+dy)) && !board.find((x+dx),(y+dy)).has_pawn  && !((dx == 0) && (dy == 0))&& !vectorFind(board.find((x),(y)).unavailables,board.find((x+dx),(y+dy))) ){
				moves.push(board.find(x+dx,y+dy));
			}
		}
	}


	return moves;
}





//lists the possible moves of the knight in the given cell of the board 
chessStack knightMoves(const Cell & knight, const chessStack & board){
	chessStack moves;
	int x = knight.x_coordinate;
	int y = knight.y_coordinate;
	int n = board.size;


	for(int dx = -2; dx < 3; dx++){
		for(int dy = -2; dy < 3; dy++){
			if((dx != 0 && dy!= 0) && (abs(dx) != abs(dy))){  // knight can not move linear
				if((x+dx)>0 && (y+dy)>0 && (x+dx)<(n+1) && (y+dy)<(n+1) && !board.find((x+dx),(y+dy)).has_pawn && !board.find((x+dx),(y+dy)).visited && !board.find((x+dx),(y+dy)).unavailable){
					moves.push(board.find(x+dx,y+dy));
				}
			}
		}
	}

	return moves;

}

//pawn placement function (commented versions are slower but they are useful algorithms so I kept them)
chessStack pawnPlacement (int n,chessStack & board){



	Cell curr = board.find(1,1);
	chessStack pawnstack;


	// fast version (pawn counter)
	while( board.pawnCounter()<n ){

		if(!board.find(curr.x_coordinate,curr.y_coordinate).has_pawn){
			pawnstack.push(curr);
			board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=true;

		}

		chessStack moves = pawnMovesVec(curr,board);
		if(!moves.isEmpty()){
			curr = moves.randPop();
		}
		else if(board.pawnCounter()==n){
			break;
		}
		else {
			if(pawnstack.numElements()>1){

				board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=false;
				vectorEmpty(board.find(curr.x_coordinate,curr.y_coordinate).unavailables);
				Cell unav = curr;
				pawnstack.pop();
				curr = pawnstack.top();
				board.find(curr.x_coordinate,curr.y_coordinate).unavailables.push_back(unav);
			}

			else{
				pawnstack.makeEmpty();
				board.resetPawns();
			}
		}
	}



	// fast version (no pawn counter - for loop)
	/*for(int i = 0;i<n;){

		if(!board.find(curr.x_coordinate,curr.y_coordinate).has_pawn){            
			pawnstack.push(curr);
			board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=true;
			i++;
		}

		chessStack moves = pawnMovesVec(curr,board);
		if(!moves.isEmpty()){
			curr = moves.randPop();
		}
		else if(i==n){
			break;
		}
		else {
			if(pawnstack.numElements()>=2){
				board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=false;
				vectorEmpty(board.find(curr.x_coordinate,curr.y_coordinate).unavailables);
				Cell unav = curr;
				i--;
				pawnstack.pop();
				curr = pawnstack.top();
				board.find(curr.x_coordinate,curr.y_coordinate).unavailables.push_back(unav);
			}

			else{
				pawnstack.makeEmpty();
				board.resetPawns();
				i=0;
			}
		}
	}*/


	// slow version (pawn counter)	
	/*while( board.pawnCounter()<n ){

	if(!board.find(curr.x_coordinate,curr.y_coordinate).has_pawn){
	pawnstack.push(curr);
	board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=true;

	}

	chessStack moves = pawnMoves(curr,board);
	if(!moves.isEmpty()){
	curr = moves.randPop();
	}
	else if(board.pawnCounter()==n){
	break;
	}
	else {
	if(pawnstack.numElements()>1){

	board.find(curr.x_coordinate,curr.y_coordinate).unavailable=true;
	board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=false;
	pawnstack.pop();
	curr = pawnstack.top();
	}

	else{
	pawnstack.makeEmpty();
	board.resetUnavail();
	board.resetPawns();
	}
	}
	}*/


	//slow version (no pawn counter - for loop)
	/*for(int i = 0;i<n;){
	//pawnstack.Print();
	//cout<<endl;
	if(!board.find(curr.x_coordinate,curr.y_coordinate).has_pawn){
	pawnstack.push(curr);
	board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=true;
	i++;
	}

	chessStack moves = pawnMoves(curr,board);
	if(!moves.isEmpty()){
	curr = moves.randPop();
	}
	else if(i==n){
	break;
	}
	else {
	if(pawnstack.numElements()>=2){
	board.find(curr.x_coordinate,curr.y_coordinate).unavailable=true;
	board.find(curr.x_coordinate,curr.y_coordinate).has_pawn=false;
	i--;
	pawnstack.pop();
	curr = pawnstack.top();
	}

	else{
	pawnstack.makeEmpty();
	board.resetUnavail();
	board.resetPawns();
	i=0;
	}
	}
	}*/

	board.resetUnavail();
	return pawnstack;

}

//king rescue function
chessStack rescueKing (int knightX, int knightY, int kingX, int kingY,chessStack & board){
	Cell curr = board.find(knightX,knightY);
	chessStack knightstack;
	Cell king = board.find(kingX,kingY);

	chessStack kingaround = knightMoves(king,board);
	chessStack knightaround = knightMoves(curr,board);
	int countlocked1 = 0;
	int countlocked2 = 0;
	while(!kingaround.isEmpty()){
		if(knightMoves(kingaround.topAndPop(),board).isEmpty()){
			countlocked1 ++;
		}
	}
	while(!knightaround.isEmpty()){
		if(knightMoves(knightaround.topAndPop(),board).isEmpty()){
			countlocked2 ++;
		}
	}

	if(knightMoves(king,board).isEmpty() || countlocked1 == knightMoves(king,board).numElements() ){
		return knightstack;
	}

	else if(knightMoves(curr,board).isEmpty() || countlocked2 == knightMoves(curr,board).numElements() ){
		return knightstack;
	}

	else{
		while(!(curr.x_coordinate == king.x_coordinate && curr.y_coordinate == king.y_coordinate)){

			if(!board.find(curr.x_coordinate,curr.y_coordinate).visited){
				knightstack.push(curr);
				board.find(curr.x_coordinate,curr.y_coordinate).visited=true;
			}

			chessStack moves = knightMoves(curr,board);
			if(!moves.isEmpty()){
				curr = moves.randPop();
			}
			else if(curr.x_coordinate == king.x_coordinate && curr.y_coordinate == king.y_coordinate){
				break;
			}
			else {
				if(knightstack.numElements()>1){

					board.find(curr.x_coordinate,curr.y_coordinate).unavailable=true;
					board.find(curr.x_coordinate,curr.y_coordinate).visited=false;
					knightstack.pop();
					curr = knightstack.top();
				}

				else{
					knightstack.makeEmpty();
					board.resetUnavail();
					board.resetVisited();
				}
			}
		}
		knightstack.push(curr);
	}

	return knightstack;

}

int main(){

	srand((unsigned)time(0));


	int n;
	int p;


	cout<<"Please enter the size of the board (n): ";
	cin>>n;

	//input check for n
	while(n<5 || n>10){
		cout<<"The size of the board (n) must be (4<n<11)"<<endl;
		cout<<"Please enter the size of the board (n) again: ";
		cin>>n;
	}


	cout<<"Please enter the number of the pawns (p): ";
	cin>>p;
	//input check for p
	while(p>(3*n)|| p<0){
		cout<<"The number of the pawns (p) must be (0<=p<=3n)"<<endl;
		cout<<"Please enter the number of the pawns (p) again: ";
		cin>>p;
	}

	chessStack board(n);

	//pawn placement
	chessStack pawnplaces = pawnPlacement(p,board);
	cout<<"The matrix:"<<endl;
	board.Print();

	int a1,a2,k1,k2;

	cout<<"Enter the position of knight(x,y): ";
	cin>>a1>>a2;
	//input check for knight
	while( (a1<1) || (a2<1) || (a1>n) || (a2>n) || board.find(a1,a2).has_pawn){
		cout<<"This position is not available! Enter the position of knight(x,y): ";
		cin>>a1>>a2;
	}
	cout<<"Enter the position of king(x,y): ";
	cin>>k1>>k2;
	//input check for king
	while( (k1<1) || (k2<1) || (k1>n) || (k2>n) || board.find(k1,k2).has_pawn){
		cout<<"This position is not available! Enter the position of king(x,y): ";
		cin>>k1>>k2;
	}

	//rescue the king
	chessStack knightpath = rescueKing(a1,a2,k1,k2,board);
	knightpath.reverse();

	bool noWay = false;
	if(knightpath.isEmpty()){
		cout<<"No way!"<<endl;
		noWay = true;;
	}
	else{
		cout<<"Path found, see the output file!"<<endl;
		noWay = false;
	}

	//output file!!

	ofstream file ("output.txt");

	file << "N = "<< n<<endl<<"P = "<<p<<endl<<endl;

	file << "Pawn Locations"<<endl;

	pawnplaces.ostreamer(file);
	file<<endl;

	file << "Knight = "<< a1<<" "<<a2<<endl<<"King = "<< k1<<" "<<k2<<endl<<endl;
	if(noWay){
		file<<"No Way!"<<endl;
	}
	else{
		knightpath.ostreamer(file);
	}
	file<<endl;

	file<<"Matrix"<<endl;
	board.ostreamer(file);
	
	
	return 0;
}