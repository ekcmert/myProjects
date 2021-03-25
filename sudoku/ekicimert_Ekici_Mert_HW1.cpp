#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

//Mert Ekici 26772 CS204 HW1

//struct for X's that stores their coordinates and possible values 
struct variable{

	pair<int,int> coordinates;
	vector<int> pvalues;

};

//remove the given integer from the given sorted vector<int>
void vectorremove(vector<int> & nums,const int & num){

	for (int i = 0; i < nums.size(); i++){
		if (nums[i] == num){
			int loc = i;
			while(loc < nums.size()-1){
				nums[loc] = nums[loc+1];
				loc++;
			}
			nums.pop_back();
		}
	}
}

//string to integer function
int toint(const string & s){

	int num;
	istringstream a(s);
	a>>num;
	return num;

}

//integer to string function
string tostr(const int & n)

{
	ostringstream output;
	output << n;   
	return output.str();
}

// matrix printer
void printmatrix(const vector<vector<string> > & m){

	for(int i = 0; i < m.size(); i++){
		for(int a = 0; a < m[i].size(); a++){
			cout << m[i][a] <<" ";
		}
		cout<<endl;
	}
} 

// function for collecting all X's in one vector as pairs of coordinates and their possible values
vector<variable> findX(const vector<vector<string> > & b){


	vector<variable> allX;

	for(int i = 0; i < b.size(); i++){
		for(int a=0; a < b[i].size(); a++){
			if (b[i][a] == "X"){
				variable var;
				var.coordinates.first = i;
				var.coordinates.second = a;
				for (int m = 1; m <= b.size(); m++){
					var.pvalues.push_back(m);
				}
				allX.push_back(var);
			}
		}
	}

	return allX;
}

//remove the row elements from the possible values of this row's variables
void rowcheck(const vector<vector<string> > & b, vector<variable>  & v){

	//loop for all elements of matrix
	for(int x = 0; x < b.size(); x++){
		for(int y=0; y < b[x].size(); y++){

			// when the element is not a variable
			if (b[x][y] != "X"){
				for(int i = 0; i < v.size(); i++){

					// remove this element from the possible values of the xth row's variables
					if ( v[i].coordinates.first == x ){
						vectorremove(v[i].pvalues, toint(b[x][y]));
					}
				}
			}
		}
	}
} 

//remove the column elements from the possible values of this columb's variables
void columncheck(const vector<vector<string> > & b, vector<variable>  & v){

	//loop for all elements of matrix
	for(int y = 0; y < b[0].size(); y++){
		for(int x = 0; x < b.size(); x++){

			// when the element is not a variable
			if(b[x][y] != "X"){
				for(int i = 0; i < v.size(); i++){

					// remove this element from the possible values of the yth column's variables
					if ( v[i].coordinates.second == y ){
						vectorremove(v[i].pvalues, toint(b[x][y]));
					}
				}
			}
		}
	}
}

//check for same value for every row and column return an integer accordingly
int samevaluecheck(const vector<vector<string> > & b){

	// row same value check
	for (int i=0; i < b.size(); i++){
		for (int a=0; a< b[i].size(); a++){
			int count = 0;
			for(int k=0; k< b[i].size(); k++){
				if(b[i][a] == b[i][k] && b[i][a] != "X"){
					count ++;
				}
			}
			if (count>1){
				return 1;
			}
		}
	}

	// column same value check
	for (int i=0; i < b[0].size(); i++){
		for (int a=0; a< b.size(); a++){
			int count = 0;
			for(int k=0; k< b.size(); k++){
				if(b[a][i] == b[k][i] && b[a][i] != "X"){
					count ++;
				}
			}
			if (count>1){
				return 2;
			}
		}
	}

	return 3;

}

//check for constraints and change the possible values accordingly
bool constraintcheck(const vector<vector<string> > & b, const vector<vector<string> > & c, vector<variable>  & v){

	// constraint data
	for (int i = 0; i < c.size(); i++){
		// if constraint coordinates equivalent in board matrix is not a "X"
		if(b[toint(c[i][0])-1][toint(c[i][1])-1] != "X" && b[toint(c[i][2])-1][toint(c[i][3])-1] != "X"){
			// if board numbers do not match constraints
			if ( toint(b[toint(c[i][0])-1][toint(c[i][1])-1]) <= toint(b[toint(c[i][2])-1][toint(c[i][3])-1]) ){
				return false;
			}
		}

		// if one of the given locations is a variable remove the "not possible values"
		else if( b[toint(c[i][0])-1][toint(c[i][1])-1] == "X" ){
			for(int k = 0; k < v.size(); k++){
				if (v[k].coordinates.first == toint(c[i][0]) - 1 && v[k].coordinates.second == toint(c[i][1])-1 ){
					for(int r = 0; r < v[k].pvalues.size(); r++){
						if(v[k].pvalues[r] <= toint(b[toint(c[i][2])-1][toint(c[i][3])-1])){
							vectorremove(v[k].pvalues,v[k].pvalues[r]);
						}
					}
				}
			}
		}
		else if( b[toint(c[i][2])-1][toint(c[i][3])-1] == "X" ){
			for(int k = 0; k < v.size(); k++){
				if (v[k].coordinates.first == toint(c[i][2])-1 && v[k].coordinates.second == toint(c[i][3])-1){
					for(int r = 0; r < v[k].pvalues.size(); r++){
						if(v[k].pvalues[r] >= toint(b[toint(c[i][0])-1][toint(c[i][1])-1])){
							vectorremove(v[k].pvalues,v[k].pvalues[r]);
						}
					}
				}
			}
		}

	}
	return true;
}

//check possible values of the variables in unknowns vector if there is a variable with more than one possible value return false
bool onecheck(const vector<variable>  & v){

	for (int i = 0; i < v.size(); i++){
		if ( v[i].pvalues.size() > 1 ){
			return false;
		}
	}

	return true;
}

//check given vector's size 
bool sizecheck(const vector<vector<string> > & v,const int & size){


	if (v.size() != size){
		return false;
	}

	else{
		for (int i = 0; i < v.size(); i++){
			if (v[i].size() != size){
				return false;
			} 
		}
	}

	return true;
} 

//check if there is an element which is greater than it's size and write it into wrong num
bool valuecheck(const vector<vector<string> > & v, const int & size, int & wrongnum){

	for(int i = 0; i < v.size(); i++){
		for(int a = 0; a < v[i].size(); a++){
			if(v[i][a] != "X"){
				if (toint(v[i][a]) > size || toint(v[i][a]) < 1){
					wrongnum = toint(v[i][a]);
					return false;
				}
			}
		}
	}

	return true;
}

//replacement X's and their true values
vector<vector<string> > plugX(vector<vector<string> > & b, const vector<variable>  & v){

	for (int i = 0; i < v.size(); i++){

		if (v[i].pvalues.size() == 1){
			b[v[i].coordinates.first][v[i].coordinates.second] = tostr(v[i].pvalues[0]);
		}
	}
	return b;
}

//decipher by using rowcheck, columncheck and constraintcheck functions iteratively in a loop 
void decipher(vector<vector<string> > & b, const vector<vector<string> > & c, vector<variable>  & v){

	// I determined the limit of loop as boardsize^2 because it is the max number of possible variables and every step of this loop can eliminate at least one variable (if it is possible with the given data)
	for(int i = 0; i < (b.size() * b.size()); i++){
		rowcheck(b,v);
		columncheck(b,v);
		constraintcheck(b,c,v);

		plugX(b,v);

		//there might be found X's, execute the same process again

		rowcheck(b,v);
		columncheck(b,v);
		constraintcheck(b,c,v);

		plugX(b,v);
	}
}

int main(){

	ifstream in;
	string filename;
	vector<vector<string> > board;
	vector<vector<string> > constr;
	vector<variable> unknowns; //vector of variables(struct)
	int boardsize;
	int constraintsize;
	int exceednum = 0; //integer for the exceeding values in board and constraint matrices (in order to print it as an error)

	// BOARD READ ****************************************

	cout<<"Enter the board file: ";
	cin>>filename;

	in.open(filename.c_str());

	// file check

	if (in.fail())
		cout << "There is an error opening that file" << endl;

	else{
		// matrix of board 

		string s;
		int linenum = 0;
		while(getline(in,s)){

			istringstream istr(s);
			vector <string> row;

			if (linenum == 0){
				istr>>boardsize;
			}
			else{
				string element;
				while(istr>>element){
					row.push_back(element);
				}
				board.push_back(row);
			}
			linenum++;		
		}
	}
	in.close();
	//printmatrix(board); (this part was used for debugging issues)

	// CONSTR READ ****************************************

	cout<<"Enter the constraint file: ";
	cin>>filename;

	in.open(filename.c_str());

	// file check

	if (in.fail())
		cout << "There is an error opening that file" << endl;

	else{
		// matrix of constraint

		string s;
		int linenum = 0;
		while(getline(in,s)){
			istringstream istr(s);
			vector <string> row;

			if (linenum == 0){
				istr>>constraintsize;
			}
			else{
				string element;
				while(istr>>element){
					row.push_back(element);
				}
				constr.push_back(row);
			}
			linenum++;		
		}
	}
	in.close();
	//printmatrix(constr); (this part was used for debugging issues)

	// PROPERTY CHECKs ***************************************


	//size check

	if(!sizecheck(board,boardsize)){
		cout<<"File contains more lines than size"<<endl;
	}

	//value check

	else if(!valuecheck(board,boardsize,exceednum)){
		cout<<"Value "<< exceednum << " is out of matrix size"<<endl;
	}


	//constraintsize check

	else if (constr.size() != (constraintsize)){
		if (constr.size() < (constraintsize)){
			cout << "There are less constraints then previously stated"<<endl;
		}
		else if (constr.size() > (constraintsize)){
			cout << "There are more constraints then previously stated"<<endl; 
		}
	}

	//Values of the coordinates must be between 1 and n check

	else if (!valuecheck(constr,boardsize,exceednum)){
		cout<<"Value "<< exceednum << " is out of matrix size."<<endl;
	}

	//There are more than one same values in the same column or row check

	else if(samevaluecheck(board)!=3){
		if(samevaluecheck(board)==1){
			cout<<"There is a value repeating in same row"<<endl;
		}
		else if(samevaluecheck(board)==2){
			cout<<"There is a value repeating in same column"<<endl;
		}
	}


	else{
		// vectors of unknowns
		unknowns = findX(board);


		//The constraints must hold check

		if(!constraintcheck(board,constr,unknowns)){
			cout<<"Constraints do not match board"<<endl;
		}

		//******************************************* FINAL SHOT

		else{
			decipher(board,constr,unknowns);

			//each empty cell must have only one possible value check

			if(!onecheck(unknowns)){
				cout<<"X's can have more than 1 value"<<endl;
			}

			else{
				cout<<"Files are in correct format"<<endl;
				printmatrix(board);
			}
		}
	}


	return 0;
}