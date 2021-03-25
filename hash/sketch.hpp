#ifndef CSKECTH_H
#define CSKECTH_H

#include <iostream>
#include <algorithm>
#include <cstring>
#include <math.h>
#include "hasher.hpp"

//Mert Ekici 26772 CS204 HW5

class Sketch{

 protected:
  unsigned no_rows;
  unsigned no_cols;
  StrHash* hashes;
  long long int* table;
  double time;
  
  Sketch(double epsilon, double delta){
    no_rows = (log(1 / delta) / log(2));
    no_cols = (2 / epsilon);
    
    //prime column count
    bool found = false;
    
    while(!found){
		found = true;

		for(unsigned i = 2; i <= sqrt(no_cols); i++){
			if(no_cols % i == 0){
				found = false;
				no_cols++;
			}
		}
	}

	table = new long long int[this->no_rows * this->no_cols];

	hashes = new StrHash[no_rows];
	for(unsigned i = 0; i < no_rows; i++){
		hashes[i].set_parameters(no_cols);
	}

    time = 0;
    
    std::cout << "A sketch with " << this->no_rows << " rows and " << this->no_cols << " columns is created" << std::endl;    
  }

 public:

  long long int get(const unsigned& i, const unsigned& j) const {
    return table[i * no_cols + j];
  }

  void add(const unsigned& i, const unsigned& j, const int& val){
    table[i * no_cols + j] += (long long int)val;
  }

  virtual void insert_to_row(const std::string& data, unsigned row_id) = 0;
  virtual void insert(const std::string& data) = 0;
  virtual long long int query(const std::string& data) const = 0;

  virtual std::string name(){
    return "Sketch";
  }

  void reset(){
    memset(table, 0, sizeof(long long)*no_rows*no_cols);
  }

  unsigned get_no_rows(){
    return no_rows;
  }

  unsigned get_no_cols(){
    return no_cols;
  }

  double getError(std::pair<std::string, int> freqs[]){
    double err = 0.0;

    for(int i = 0; i < 1000; i++){
      std::string curr = freqs[i].first;
      long long int actual = freqs[i].second;
      long long int guess = query(curr);

      double cerr = ((double)(abs(guess-actual)) / actual);
      err += cerr * cerr;
    }
    return sqrt(err / 1000);
  }
  
  void print() {
	  for(unsigned i = 0; i < no_rows; i++) {
		  for(unsigned j = 0; j < no_cols; j++) {
			  std::cout << get(i, j) << " ";
		  }
		  std::cout << std::endl;
	  }
  }
  

  void add_to_time(double partial){
    this->time += partial;
  }

  double get_time(){
    return this->time;
  }

  void reset_time(){
    this->time = 0;
  }
  
  ~Sketch(){
    delete[] table;
    delete[] hashes;
  }
};


 
class FreqSketch: public Sketch{
public:

	//Constructor
	FreqSketch(double epsilon, double delta, int id):Sketch(epsilon,delta){
		//TO DO: IMPLEMENT DERIVED SKETCHES
		reset();
		if (id == 0){
			std::cout <<"--> It is a Count "<< Sketch::name()<<"!"<<std::endl;
		}
		else if (id == 1){
			std::cout <<"--> It is a Count-Min "<< Sketch::name()<<"!"<<std::endl;
		}
		else{
			std::cout <<"--> It is Count-Min-Mean "<< Sketch::name()<<"!"<<std::endl;
		}
	}


	//Destructor
	~FreqSketch(){}
};



class CountSketch: public FreqSketch{
public:

	//Constructor
	CountSketch(double epsilon, double delta, int id = 0): FreqSketch(epsilon,delta,id){

		results = new int[no_rows];
		g_hashes = new StrHash[no_rows];
		
		//g_hashes colsize should not be zero
		for(unsigned i = 0; i < no_rows; i++){
			g_hashes[i].set_parameters(no_cols);
		}
	}

	//CountSketch insert_to_row
	virtual void insert_to_row(const std::string& data, unsigned row_id){
		unsigned col_id = hashes[row_id].hash(data);
		int contrib = 2 * int(g_hashes[row_id].hash(data)%2) - 1;
		add(row_id,col_id,contrib);
		
	
	}
	//CountSketch insert
	virtual void insert(const std::string& data) {
		for(unsigned i = 0; i < no_rows; i++){
			insert_to_row(data,i);
		}
	
	}

	//CountSketch query
	virtual long long int query(const std::string& data) const {
		unsigned h_col;
		long long int coef = 0;
		memset(results,0,sizeof(int)*no_rows);
		for(unsigned i = 0; i < no_rows; i++){
			h_col = hashes[i].hash(data);
			coef = 2 * int(g_hashes[i].hash(data)%2) - 1;
			results[i] = get(i,h_col) * coef;
			if (results[i] < 0){
				results[i] = 0;
			}
		}
		std::sort(results,results + no_rows);
		return results[no_rows / 2];
	}



	//overriden name function
	virtual std::string name(){
		return "CS";
	}

	//Destructor (frees dynamic memory used in this class)
	~CountSketch(){
		delete[] g_hashes;
		delete[] results;
	}

//private data
private:
	StrHash* g_hashes;
	int* results;
};


class CountMinSketch: public FreqSketch{
public:
	//Constructor
	CountMinSketch(double epsilon, double delta, int id = 1): FreqSketch(epsilon,delta,id){
	}

	//CountMinSketch insert_to_row
	virtual void insert_to_row(const std::string& data, unsigned row_id){
		unsigned col_id = hashes[row_id].hash(data);
		add(row_id,col_id,1);
	}

	//CountMinSketch insert
	virtual void insert(const std::string& data){
		for(unsigned i = 0; i < no_rows; i++){
			insert_to_row(data,i);
		}
	}

	//CountMinSketch query
	virtual long long int query(const std::string& data) const{
		long long int r_freq, freq = std::numeric_limits<long long int>::max();
		unsigned h_col = 0;
		for(unsigned i = 0; i < no_rows; i++){
			h_col = hashes[i].hash(data);
			r_freq = get(i,h_col);
			if (r_freq < freq){
				freq = r_freq;
			}
		}
		return freq;
	}
	//overriden name function
	virtual std::string name(){
		return "CMS";
	}
	//Destructor 
	~CountMinSketch(){}
};


class CountMinMeanSketch: public CountMinSketch{
public:

	//Constructor
	CountMinMeanSketch(double epsilon, double delta, int id = 2): CountMinSketch(epsilon,delta,id){
		results = new int[no_rows];
	}
	//CountMinMeanSketch insert_to_row
	virtual void insert_to_row(const std::string& data, unsigned row_id){
		unsigned col_id = hashes[row_id].hash(data);
		add(row_id,col_id,1);
	}
	//CountMinMeanSketch insert
	virtual void insert(const std::string& data){
		for(unsigned i = 0; i < no_rows; i++){
			insert_to_row(data,i);
		}
	}
	//CountMinMeanSketch query
	virtual long long int query(const std::string& data) const{
		long long int no_stream, r_freq, noise = 0;
		unsigned h_col = 0;
		memset(results,0,sizeof(int)*no_rows);
		for(unsigned i = 0; i < no_rows; i++){
			h_col = hashes[i].hash(data);
			r_freq = get(i,h_col);
			no_stream = 0;
			for(unsigned j = 0; j < no_cols; j++){
				no_stream += get(i,j);
			}
			noise = ((no_stream - r_freq) / (no_cols - 1));
			if (r_freq > noise){
				results[i] = r_freq - noise;
			}
		}
		std::sort(results,results + no_rows);
		return results[no_rows / 2];
	}

	//overriden name function
	virtual std::string name(){
		return "CMMS";
	}

	//Destructor (frees dynamic memory used in this class)
	~CountMinMeanSketch(){
		delete[] results;
	}

//private data
private:
	int* results;
};


#endif