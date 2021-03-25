#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;

#define _LEAKCHECK
#ifdef _LEAKCHECK
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#undef _LEAKCHECK


//   --- Mert Ekici 26772 CS204-HW2 ---


//structs of stop, line, path
struct metroStop {
	string metroStopName;
	metroStop *left;
	metroStop *right;  

	//extra members
	bool istransfer;
	bool ispassed;

	//default constructor
	metroStop::metroStop ():
		ispassed(false),
		istransfer(false)
	{}

	//constructor with 3 parameters
	metroStop::metroStop(string s, metroStop * l, metroStop * r)
		: metroStopName(s), left(l), right(r), ispassed(false), istransfer(false)
	{};

};

struct metroLine {
	string metroNumber;
	metroLine *next;  
	metroStop *metroStops;

	vector<double>costs;

	//default constructor
	metroLine::metroLine(){}

	//constructor with 3 parameters
	metroLine::metroLine(string s, metroLine * n, metroStop * m)
		: metroNumber(s), next(n), metroStops(m)
	{};
};

struct path {

	double cost;
	int hop;
	vector<string> pathsteps;
	string lasttransfer;

	path::path() : 
		cost(0),
		hop(0){}

};

//menu functions
bool consistencyCheck(metroLine* head) {
	metroLine* currBL = head;
	while(currBL) {
		metroStop* currBS = currBL->metroStops;
		while(currBS) {
			metroStop* rightBS = currBS->right;
			if(rightBS && rightBS->left != currBS) {
				cout << "Inconsistency for " 
					<< currBL->metroNumber 
					<< " " 
					<< currBS->metroStopName << endl;
				return false;
			}
			currBS = currBS->right;
		}
		currBL = currBL->next;
	}
	return true;
}

void printMainMenu() {
	cout << endl;
	cout <<"I***********************************************I"<<endl
		<<"I               0 - EXIT PROGRAM                I"<<endl
		<<"I               1 - PRINT LINES                 I"<<endl
		<<"I               2 - ADD METRO STOP              I"<<endl
		<<"I               3 - DELETE METRO STOP           I"<<endl
		<<"I               4 - PATH FINDER                 I"<<endl
		<<"I***********************************************I"<<endl
		<<">>";
	cout << endl;
}  

//print functions for linked metro lines
void printsingleLine(metroLine* line){


	cout<<setw(5)<<left<<line->metroNumber+":";

	metroStop* currS = line->metroStops;

	int i = 0;

	while(currS){
		cout<<currS->metroStopName;
		if (i < line->costs.size()){
			cout<<"<-"<<line->costs[i]<<"->";
		}
		i++;
		currS = currS->right;
	}
	cout<<endl;
}

void printmetroLinesDetailed(metroLine* head){

	metroLine* currL = head;

	while(currL){
		printsingleLine(currL);
		currL = currL-> next;
	}
}

//exist checks for lines and stops
bool existcheckLine(metroLine* head, string lname){ // metroline existence check

	metroLine* currL = head;
	while(currL){
		if(currL->metroNumber != lname){
			currL = currL->next;
		}
		else{
			return true;
		}
	}
	return false;

}

bool existcheckStop(metroStop* head, string sname){ // metrostop existence check

	metroStop* currS = head;
	while(currS){
		if(currS->metroStopName != sname){
			currS = currS->right;
		}
		else{
			return true;
		}
	}
	return false;

}

//finder functions
metroLine* metroLinefinder(metroLine* head, string lname){ // find the given metroline and return it

	metroLine* currL = head;
	while(currL){
		if(currL->metroNumber != lname){
			currL = currL->next;
		}
		else{
			break;
		}
	}

	return currL;
}

metroStop* metroStopfinder(metroStop* head, string sname){ // find the given metrostop and return it

	metroStop* currS = head;
	while(currS){
		if(currS->metroStopName != sname){
			currS = currS->right;
		}
		else{
			break;
		}
	}

	return currS;
}

metroLine* prevLinefinder(metroLine* head, string lname){ //find the previous line and return it

	metroLine* currL = head;
	while(currL){
		if(currL->next->metroNumber != lname){
			currL = currL->next;
		}
		else{
			break;
		}
	}

	return currL;
}

//returns the index of given stop in its line
int metroStoplocation(metroStop* head, string sname){

	metroStop* currS = head;
	int i = 0;
	while(currS){
		if(currS->metroStopName != sname){
			currS = currS->right;
			i ++;
		}
		else{
			break;
		}
	}

	return i;

}

//vector functions for the costs vector
void vectorinsertion(vector<double> & v, int index, double value){

	v.push_back(value);

	double temp;

	for (int i = v.size()-1; i > index; i--){

		temp = v[i];
		v[i] = v[i-1];
		v[i-1] = temp;
	}
}

void vectorremove(vector<double> & v, int index){

	for (int i = index; i < v.size()-1; i++){
		v[i]=v[i+1];
	}

	v.pop_back();
}

//vector remove for string vector
void vectorremoves(vector<string> & v, int index){

	for (int i = index; i < v.size()-1; i++){
		v[i]=v[i+1];
	}

	v.pop_back();
}

// add and delete metro stop functions
void addmetroStop(metroLine* head){

	string linename;
	cout<<"Enter the name of the metro line to insert a new metro stop (0 for main menu)"<<endl;
	cin>>linename;

	if(linename == "0"){
		return;
	}

	else if (!existcheckLine(head,linename)){
		cout<<"Metro line cannot be found. Going back to previous menu."<<endl;
	}

	else{

		metroLine* selectedLine = metroLinefinder(head,linename);
		cout<<"The metro line information is shown below"<<endl;
		printsingleLine(selectedLine);

		string stopname;

		cout<<"Enter the name of the new metro stop"<<endl;
		cin>>stopname;

		if(existcheckStop(selectedLine->metroStops,stopname)){
			cout<<"Metro stop already exists. Going back to previous menu."<<endl;
		}

		else{

			string prevstop;
			double cost1, cost2;
			cout<<"Enter the name of the previous metro stop for the new one (0 to put the new one as the first metro stop)"<<endl;
			cin>>prevstop;

			cout<<"Enter new cost 1"<<endl;
			cin>>cost1;

			cout<<"Enter new cost 2"<<endl;
			cin>>cost2;

			while(!existcheckStop(selectedLine->metroStops,prevstop) && prevstop != "0"){ // 0 is not a stopname !!
				cout<<"Metro stop does not exist. Typo? Enter again (0 for main menu)"<<endl;
				cin>>prevstop;
				if(prevstop == "0"){
					return;
				}
			}

			if(prevstop == "0"){ // if first metro stop

				metroStop* newStop = new metroStop(stopname,nullptr,selectedLine->metroStops);
				newStop->right->left = newStop;
				selectedLine->metroStops = newStop;
				vectorinsertion(selectedLine->costs,0,cost1);
				printsingleLine(selectedLine);
			}
			else if(metroStopfinder(selectedLine->metroStops,prevstop)->right == nullptr){ // if previous stop is tail 
				metroStop* newStop = new metroStop(stopname,metroStopfinder(selectedLine->metroStops,prevstop),nullptr);
				newStop->left->right = newStop;
				selectedLine->costs.push_back(cost1);
				printsingleLine(selectedLine);
			}
			else{
				metroStop* newStop = new metroStop(stopname,metroStopfinder(selectedLine->metroStops,prevstop),metroStopfinder(selectedLine->metroStops,prevstop)->right);
				newStop->left->right = newStop;
				newStop->right->left = newStop;

				int prevlocation = metroStoplocation(selectedLine->metroStops,prevstop);
				vectorinsertion(selectedLine->costs,prevlocation,cost1); // add cost1 given location
				selectedLine->costs[prevlocation+1] = cost2; // replace cost2 with the old cost 

				printsingleLine(selectedLine);

			}

		}


	}

}

void deletemetroStop(metroLine* head){

	string linename;
	cout<<"Enter the name of the metro line to delete a new metro stop (0 for main menu)"<<endl;
	cin>>linename;

	if(linename == "0"){
		return;
	}

	else if (!existcheckLine(head,linename)){
		cout<<"Metro line cannot be found. Going back to previous menu."<<endl;
	}

	else{

		metroLine* selectedLine = metroLinefinder(head,linename);
		cout<<"The metro line information is shown below"<<endl;
		printsingleLine(selectedLine);

		string stopname;

		cout<<"Enter the name of the metro stop to delete"<<endl;
		cin>>stopname;

		if(stopname=="0"){
			return;
		}
		while(!existcheckStop(selectedLine->metroStops,stopname) && stopname != "0"){ // 0 is not a stopname !!
			cout<<"Metro stop cannot be found. Enter the name of the metro stop to delete (0 for main menu)"<<endl;
			cin>>stopname;
			if(stopname == "0"){
				return;
			}
		}

		if(selectedLine->metroStops->metroStopName == stopname){ //if delete_stop is head


			if(selectedLine->metroStops->right==nullptr){ // the last station's deletion eliminates the whole line

				delete selectedLine->metroStops;
				prevLinefinder(head,linename)->next = selectedLine->next;
				delete selectedLine;
			}

			else{

				metroStop* delP = selectedLine->metroStops;
				selectedLine->metroStops = selectedLine->metroStops->right;
				selectedLine->metroStops->left = nullptr;
				delete delP;

				vectorremove(selectedLine->costs,0);
				printsingleLine(selectedLine);
			}
		}

		else if(metroStopfinder(selectedLine->metroStops,stopname)->right == nullptr){ //if delete_stop is tail

			metroStopfinder(selectedLine->metroStops,stopname)->left->right=nullptr;
			delete metroStopfinder(selectedLine->metroStops,stopname);

			vectorremove(selectedLine->costs,selectedLine->costs.size()-1);
			printsingleLine(selectedLine);

		}

		else{

			int deletelocation = metroStoplocation(selectedLine->metroStops,stopname);
			metroStop* delP = metroStopfinder(selectedLine->metroStops,stopname);
			metroStopfinder(selectedLine->metroStops,stopname)->right->left = metroStopfinder(selectedLine->metroStops,stopname)->left; // (1)           we first did (1), because (2) breaks metroStopfinder function which uses 'right's to find given named stop
			metroStopfinder(selectedLine->metroStops,stopname)->left->right = metroStopfinder(selectedLine->metroStops,stopname)->right; // (2)

			delete delP;

			selectedLine->costs[deletelocation-1] = selectedLine->costs[deletelocation-1] + selectedLine->costs[deletelocation];
			vectorremove(selectedLine->costs,deletelocation);
			printsingleLine(selectedLine);

		}
	}
}

//returns the line of the given stop
metroLine* lineofstop(metroStop* stop, metroLine* head){

	metroLine* currL = head;

	while(currL){
		metroStop* currS = currL->metroStops;
		while(currS){
			if (currS == stop){
				return currL;
			}
			currS = currS->right;
		}
		currL = currL->next;
	}

	return nullptr;

}

//gets all transferstops of a given stop in a metroStop* vector
vector<metroStop*> getalltransfer(metroLine* head, string stopname){

	vector<metroStop*> alltransfers;
	metroLine* currL = head;
	while(currL){
		metroStop* currS = currL->metroStops;
		while(currS){
			if(currS->metroStopName==stopname){
				alltransfers.push_back(currS);
			}
			currS = currS->right;
		}
		currL = currL->next;
	}

	return alltransfers;

}

//marks the all transfer stops in the lines
void transferStopmaker(metroLine* head){

	metroLine* currL = head;


	while(currL){

		metroStop* currS = currL->metroStops;
		while(currS){

			//check other lines and stops
			metroLine* checkL = head;
			while(checkL){
				metroStop* checkS = checkL->metroStops;
				while(checkS){
					if (checkS->metroStopName == currS->metroStopName && checkL != currL){
						currS->istransfer = true;
					}
					checkS = checkS->right;
				}
				checkL = checkL->next;
			}

			currS = currS->right;
		}

		currL = currL->next;
	}


}

//resets the all passed stops (ispassed = false) after the use of pathmakers
void resetpassedstops (metroLine* head){

	metroLine* currL = head;
	while(currL){
		metroStop* currS = currL->metroStops;
		while(currS){
			currS->ispassed = false;
			currS = currS->right;
		}
		currL = currL->next;
	}

}

//finds the given named stop and returns it
metroStop* findpoint(metroLine* head, string stopname){

	metroLine* currL = head;

	while(currL){
		if(existcheckStop(currL->metroStops,stopname)){
			return metroStopfinder(currL->metroStops,stopname);
		}
		else{
			currL = currL->next;
		}
	}

	return nullptr;
}

//reverse pathsteps vector of the given path (for reverse pathmakers)
path reversepath(const path & pth, const vector<string> & pathsteps){

	vector<string> reversedpathsteps;
	path reversedpath = pth;
	int temploc = pathsteps.size()-1;
	for (int i = pathsteps.size()-1 ; i >= 0 ; i--) {
		if(pathsteps[i][pathsteps[i].size()-1] == ':'){
			reversedpathsteps.push_back(pathsteps[i]);
			for(int a = temploc; a > i; a--){
				reversedpathsteps.push_back(pathsteps[a]);
			}
			temploc = i-1;
		}
	}
	reversedpath.pathsteps = reversedpathsteps;
	return reversedpath;
}

// functions that places the paths (with the allowed hop number) to the pathvector, there are five of them for the different cases
void pathmaker1(vector<path> & paths, path & pth, metroLine* head, metroStop* & currP, string startP, string endP, int maxhop){


	if( pth.hop > maxhop) { // exit condition #1 hop exceed

		return;

	}

	else if ( currP->metroStopName == endP ){ // exit condition #2 path found


		pth.pathsteps.push_back(currP->metroStopName);
		paths.push_back(pth);
		return;
	}

	else{

		if(currP->istransfer){
			if(!currP->ispassed){

				for(int i = 0; i < getalltransfer(head,currP->metroStopName).size(); i++){
					path temp = pth;
					if(!getalltransfer(head,currP->metroStopName)[i]->ispassed){
						if(currP->metroStopName!=startP){
							pth.pathsteps.push_back(currP->metroStopName);
							pth.pathsteps.push_back(lineofstop(getalltransfer(head,currP->metroStopName)[i],head)->metroNumber+":");
							pth.hop++;
							if(pth.lasttransfer == currP->metroStopName){
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								pth.hop--;
							}
						}
						pth.lasttransfer= currP->metroStopName;
						currP->ispassed = true;
						pathmaker1(paths,pth,head,getalltransfer(head,currP->metroStopName)[i],startP,endP,maxhop);
						pth = temp;
					}

				}

			}

		}

		if(currP->left && !currP->left->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)-1];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker1(paths,pth,head,currP->left,startP,endP,maxhop);
			pth = temp;
		}
		if(currP->right && !currP->right->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker1(paths,pth,head,currP->right,startP,endP,maxhop);
			pth = temp;
		}

	}
}

void pathmaker2(vector<path> & paths, path & pth, metroLine* head, metroStop* & currP, string startP, string endP, int maxhop){


	if( pth.hop > maxhop) { // exit condition #1 hop exceed

		return;

	}

	else if ( currP->metroStopName == endP ){ // exit condition #2 path found


		pth.pathsteps.push_back(currP->metroStopName);
		paths.push_back(pth);
		return;
	}

	else{

		if(currP->istransfer){
			if(!currP->ispassed){
				currP->ispassed = true;
				for(int i = 0; i < getalltransfer(head,currP->metroStopName).size(); i++){
					path temp = pth;
					if(!getalltransfer(head,currP->metroStopName)[i]->ispassed){
						if(currP->metroStopName!=startP){
							pth.pathsteps.push_back(currP->metroStopName);
							pth.pathsteps.push_back(lineofstop(getalltransfer(head,currP->metroStopName)[i],head)->metroNumber+":");
							pth.hop++;
							if(pth.lasttransfer == currP->metroStopName){
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								pth.hop--;
							}
						}
						pth.lasttransfer= currP->metroStopName;
						
						pathmaker2(paths,pth,head,getalltransfer(head,currP->metroStopName)[i],startP,endP,maxhop);
						pth = temp;
					}

				}

			}

		}

		if(currP->left && !currP->left->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)-1];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker2(paths,pth,head,currP->left,startP,endP,maxhop);
			pth = temp;
		}
		if(currP->right && !currP->right->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker2(paths,pth,head,currP->right,startP,endP,maxhop);
			pth = temp;
		}

	}
}

void pathmaker3(vector<path> & paths, path & pth, metroLine* head, metroStop* & currP, string startP, string endP, int maxhop){


	if( pth.hop > maxhop) { // exit condition #1 hop exceed

		return;

	}

	else if ( currP->metroStopName == endP ){ // exit condition #2 path found


		pth.pathsteps.push_back(currP->metroStopName);
		paths.push_back(pth);
		return;
	}

	else{
		
		if(currP->istransfer){
			if(!currP->ispassed){
				currP->ispassed = true;
				for(int i = 0; i < getalltransfer(head,currP->metroStopName).size(); i++){
					path temp = pth;
					if(!getalltransfer(head,currP->metroStopName)[i]->ispassed){
						if(currP->metroStopName!=startP){
							pth.pathsteps.push_back(currP->metroStopName);
							pth.pathsteps.push_back(lineofstop(getalltransfer(head,currP->metroStopName)[i],head)->metroNumber+":");
							pth.hop++;
							if(pth.lasttransfer == currP->metroStopName){
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								pth.hop--;
							}
						}
						pth.lasttransfer= currP->metroStopName;
						
						metroStop* tcurrP = getalltransfer(head,currP->metroStopName)[i];
						if(tcurrP->left && !tcurrP->left->ispassed){
							path temp = pth;
							pth.cost += lineofstop(tcurrP,head)->costs[metroStoplocation(lineofstop(tcurrP,head)->metroStops,tcurrP->metroStopName)-1];
							if (tcurrP->metroStopName == startP){
								pth.pathsteps.push_back(lineofstop(tcurrP,head)->metroNumber+":");
							}
							pth.pathsteps.push_back(tcurrP->metroStopName);
							tcurrP->ispassed = true;
							pathmaker3(paths,pth,head,tcurrP->left,startP,endP,maxhop);
							pth = temp;
						}
						if(tcurrP->right && !tcurrP->right->ispassed){
							path temp = pth;
							pth.cost += lineofstop(tcurrP,head)->costs[metroStoplocation(lineofstop(tcurrP,head)->metroStops,tcurrP->metroStopName)];
							if (tcurrP->metroStopName == startP){
								pth.pathsteps.push_back(lineofstop(tcurrP,head)->metroNumber+":");
							}
							pth.pathsteps.push_back(tcurrP->metroStopName);
							tcurrP->ispassed = true;
							pathmaker3(paths,pth,head,tcurrP->right,startP,endP,maxhop);
							pth = temp;
						}
						pth = temp;

					}
				}
			}
		}

		if(currP->left && !currP->left->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)-1];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker3(paths,pth,head,currP->left,startP,endP,maxhop);
			pth = temp;
		}
		if(currP->right && !currP->right->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker3(paths,pth,head,currP->right,startP,endP,maxhop);
			pth = temp;
		}
		

	}
}

void pathmaker4(vector<path> & paths, path & pth, metroLine* head, metroStop* & currP, string startP, string endP, int maxhop){


	if( pth.hop > maxhop) { // exit condition #1 hop exceed

		return;

	}

	else if ( currP->metroStopName == endP ){ // exit condition #2 path found


		pth.pathsteps.push_back(currP->metroStopName);
		paths.push_back(reversepath(pth,pth.pathsteps));
		return;
	}

	else{

		if(currP->istransfer){
			if(!currP->ispassed){
				currP->ispassed = true;
				for(int i = 0; i < getalltransfer(head,currP->metroStopName).size(); i++){
					path temp = pth;
					if(!getalltransfer(head,currP->metroStopName)[i]->ispassed){
						if(currP->metroStopName!=startP){
							pth.pathsteps.push_back(currP->metroStopName);
							pth.pathsteps.push_back(lineofstop(getalltransfer(head,currP->metroStopName)[i],head)->metroNumber+":");
							pth.hop++;
							if(pth.lasttransfer == currP->metroStopName){
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								pth.hop--;
							}
						}
						pth.lasttransfer= currP->metroStopName;
						
						pathmaker4(paths,pth,head,getalltransfer(head,currP->metroStopName)[i],startP,endP,maxhop);
						pth = temp;
					}

				}

			}

		}

		if(currP->left && !currP->left->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)-1];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker4(paths,pth,head,currP->left,startP,endP,maxhop);
			pth = temp;
		}
		if(currP->right && !currP->right->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker4(paths,pth,head,currP->right,startP,endP,maxhop);
			pth = temp;
		}

	}
}

void pathmaker5(vector<path> & paths, path & pth, metroLine* head, metroStop* & currP, string startP, string endP, int maxhop){


	if( pth.hop > maxhop) { // exit condition #1 hop exceed

		return;

	}

	else if ( currP->metroStopName == endP ){ // exit condition #2 path found


		pth.pathsteps.push_back(currP->metroStopName);
		paths.push_back(reversepath(pth,pth.pathsteps));
		return;
	}

	else{

		if(currP->istransfer){
			if(!currP->ispassed){

				for(int i = 0; i < getalltransfer(head,currP->metroStopName).size(); i++){
					path temp = pth;
					if(!getalltransfer(head,currP->metroStopName)[i]->ispassed){
						if(currP->metroStopName!=startP){
							pth.pathsteps.push_back(currP->metroStopName);
							pth.pathsteps.push_back(lineofstop(getalltransfer(head,currP->metroStopName)[i],head)->metroNumber+":");
							pth.hop++;
							if(pth.lasttransfer == currP->metroStopName){
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								vectorremoves(pth.pathsteps,pth.pathsteps.size()-3);
								pth.hop--;
							}
						}
						pth.lasttransfer= currP->metroStopName;
						currP->ispassed = true;
						pathmaker5(paths,pth,head,getalltransfer(head,currP->metroStopName)[i],startP,endP,maxhop);
						pth = temp;
					}

				}

			}

		}

		if(currP->left && !currP->left->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)-1];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker5(paths,pth,head,currP->left,startP,endP,maxhop);
			pth = temp;
		}
		if(currP->right && !currP->right->ispassed){
			path temp = pth;
			pth.cost += lineofstop(currP,head)->costs[metroStoplocation(lineofstop(currP,head)->metroStops,currP->metroStopName)];
			if (currP->metroStopName == startP){
				pth.pathsteps.push_back(lineofstop(currP,head)->metroNumber+":");
			}
			pth.pathsteps.push_back(currP->metroStopName);
			currP->ispassed = true;
			pathmaker5(paths,pth,head,currP->right,startP,endP,maxhop);
			pth = temp;
		}

	}
}

//finds the mincost path of the path vector
path mincostfinder(const vector<path> & paths){

	double mincost = paths[0].cost;
	path minpath = paths[0];
	int minsize = paths[0].pathsteps.size();
	for (int i = 1; i < paths.size(); i++){
		if ( paths[i].cost < mincost ){
			minpath = paths[i];
			mincost = minpath.cost;
			minsize = paths[i].pathsteps.size();
		}
		else if (paths[i].cost == mincost){
			if(paths[i].pathsteps.size() < minsize){
				minpath = paths[i];
				mincost = minpath.cost;
				minsize = paths[i].pathsteps.size();
			}

		}
	}
	return minpath;
}

//prints the pathsteps element of the path in an organized way
void pathprinter(const path & pth, metroLine* head){


	cout<<"Best cost path:"<<endl;

	for (int i = 0; i < pth.pathsteps.size(); i++){
		if(i == 0 && existcheckLine(head, pth.pathsteps[i].substr(0,pth.pathsteps[i].length()-1))){
			cout<<setw(5)<<left<<pth.pathsteps[i];
		}
		else if(i != 0 && existcheckLine(head, pth.pathsteps[i].substr(0,pth.pathsteps[i].length()-1))){
			cout<<endl<<setw(5)<<left<<pth.pathsteps[i];
		}
		else{
			cout<<pth.pathsteps[i]<<" ";
		}
	}
	cout<<endl;
	cout<<setw(8)<<left<<"Hops:"<<pth.hop<<endl;
	cout<<setw(8)<<left<<"Cost:"<<pth.cost<<endl;

}

//collected version of path finder
void pathfinder(metroLine* head,int maxhop){ //if startP == transfer then do not cut hop at the beginning

	string SP, EP;
	cout<<"Where are you now?"<<endl;
	cin>>SP;
	cout<<"Where do you want to go?"<<endl;
	cin>>EP;

	if(findpoint(head,SP) == nullptr || findpoint(head,EP) == nullptr){
		cout<<"Metro stop does not exist in the table. Going back to previous menu."<<endl;
		return;
	}

	else{
		transferStopmaker(head);
		metroStop* locnow = findpoint(head, SP);
		metroStop* locnowrev = findpoint(head, EP);

		vector<path> truepaths;

	
		
		
		auto start = std::chrono::high_resolution_clock::now();
		
		
	// some of the pathmakers ignore the low hopped paths = double check :)
		for(int i = 0; i<=maxhop; i++){
			path currpath1;
			pathmaker1(truepaths,currpath1,head,locnow,SP,EP,i);
			resetpassedstops(head);

			path currpath2;
			pathmaker2(truepaths,currpath2,head,locnow,SP,EP,i);
			resetpassedstops(head);

			path currpath3;
			pathmaker3(truepaths,currpath3,head,locnow,SP,EP,i);
			resetpassedstops(head);
			
			path currpath4;
			pathmaker4(truepaths,currpath4,head,locnowrev,EP,SP,i);
			resetpassedstops(head);

			path currpath5;
			pathmaker5(truepaths,currpath5,head,locnowrev,EP,SP,i);
			resetpassedstops(head);
		}
		

		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;

		if(truepaths.size()!=0){
			pathprinter(mincostfinder(truepaths),head);
			cout << "Elapsed time: " << elapsed.count() << " s\n";
		}
		else{
			cout<<"No path found between "<<SP<<" and "<<EP<<endl;
			cout << "Elapsed time: " << elapsed.count() << " s\n";
		}


	}


}

//main menu
void processMainMenu(metroLine* head, int maxhop) { // add parameters to inner functions which is reachable from this function's parameters
char input;
do{
if(!consistencyCheck(head)) {
cout << "There are inconsistencies. Exit." << endl; 
return;
}
printMainMenu();
cout << "Please enter your option " << endl;
cin >> input;
switch (input) {
case '0':
cout << "Thanks for using our software" << endl;
return;
case '1':
printmetroLinesDetailed(head);
break;
case '2':
addmetroStop(head);
break;
case '3':
deletemetroStop(head);
break;
case '4':
pathfinder(head,maxhop);
break;
default:
cout << "Invalid option: please enter again" << endl;
}
} while(true);
}

//replace commas with white spaces
void commaparse(string & s){

	for (int i = 0; i < s.length(); i++){
		if (s[i] == ','){
			s[i] = ' ';
		}
	}
}

//read metroline file
void lineread(ifstream & f, metroLine* &head){ //close file //delete unnecessary like previous

	string linefile;
	cout<<"Enter the name of the metro line data file: \n";
	cin>>linefile;

	f.open(linefile.c_str());

	if (f.fail()){
		head = nullptr;
		cout << "There is an error opening that file" << endl;
	}

	else{

		string s;
		metroLine *mLineHead = head;
		metroLine* previousLine = mLineHead;
		int linenum = 0; //to make first line the head of lines
		while(getline(f,s)){

			string metroLineName;
			string metroStationName;
			string HeadStation;

			s = s.substr(0,s.length()-1); //eliminate last item '#'
			commaparse(s);
			istringstream istr(s);

			if(linenum == 0){ //head of metrolines
				istr>>metroLineName>>HeadStation;
				mLineHead->metroNumber = metroLineName;
				metroStop *mHead = new metroStop(HeadStation,nullptr,nullptr);
				mLineHead->metroStops = mHead;

				metroStop* previousStop = mHead;

				while(istr>>metroStationName){ //building metrostops

					metroStop* station = new metroStop(metroStationName,previousStop,nullptr); //adding next station with given parameters
					previousStop->right = station;
					previousStop = station;

				}
			}


			else{ //non first metrolines

				istr>>metroLineName>>HeadStation;
				metroStop *mHead = new metroStop(HeadStation, nullptr, nullptr);
				metroLine* mLine = new metroLine(metroLineName, nullptr, mHead);
				previousLine->next = mLine;
				previousLine = mLine;

				metroStop* previousStop = mHead;

				while(istr>>metroStationName){ //building metrostops

					metroStop* station = new metroStop(metroStationName,previousStop,nullptr); //adding next station with given parameters
					previousStop->right = station;
					previousStop = station;

				}
			}
			linenum++;
		}

	}

	f.close();
}

//read cost file
void costread(ifstream & f, metroLine* &head){
	string costfile;
	cout<<"Enter the name of costs file: \n";
	cin>>costfile;

	f.open(costfile.c_str());

	if (f.fail())
		cout << "There is an error opening that file" << endl;

	else if (head==nullptr) { //wrong lines file
		return;
	}

	else{

		string s;
		metroLine* currentLine = head;
		while(getline(f,s)){

			string lineName;
			double cost;
			vector<double> costs;

			commaparse(s);
			istringstream istr(s);

			istr>>lineName;

			while(istr>>cost){
				costs.push_back(cost);
			}

			currentLine->costs = costs;

			currentLine = currentLine->next;

		}
	}

	f.close();
}

//delete dynamically allocated memory (lines and stops)
void deleteLinesStops(metroLine* & head){

	metroLine * currL = head;

	while(currL){
		metroStop* currS = currL->metroStops;
		while(currS){
			metroStop* tempS = currS->right;
			delete currS;
			currS = tempS;
		}
		metroLine* tempL = currL->next;
		delete currL;
		currL = tempL;
	}
}

int main(){


	ifstream in;

	metroLine *mLineHead = new metroLine;

	lineread(in, mLineHead);
	costread(in, mLineHead);
	int maxhopnum;
	cout<<"Enter maximum number of hops:"<<endl;
	cin>>maxhopnum;

	if(mLineHead==nullptr || mLineHead->costs.size()<1){
		cout<<"Wrong Files!"<<endl;
	}

	else{
		processMainMenu(mLineHead,maxhopnum);
		deleteLinesStops(mLineHead);
	}

#ifdef _LEAKCHECK
	cout<<_CrtDumpMemoryLeaks()<<endl;
#endif
	

	return 0; 
}

