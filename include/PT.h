#ifndef __PT_H__
#define __PT_H__


#include <vector>
#include <deque> 
#include <thread>         
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <math.h>
#include <algorithm>
#include <bits/stdc++.h>
#include "NPM.h"
#include "Consumer.h"
#include "NodeMCMC.h"
#include "NodeSwap.h"
#include "ExecTime.h"


class PT{
	private:
		// PT varibles
		float tempMin_; // init temp
		float tempMax_; // end temp
		int tempL_; // temp size
		int MKL_; // MC length
		int PTL_; // number of swaps
		int tempUpdate; // number of swaps to update de temp
		int numberMachine;
		std::vector<int> changeTax;
		int numberJobs;
		int numberTools; 
		std::vector<int> capacityMagazine;
		int sum = 0;
		int totalJobTax = 0;
		int stopC_;
		std::string fileName;
		std::vector<std::vector<bool>>  toolJob;
		std::vector<std::vector<int>>  JobTools;
		std::vector<std::vector<int>> JobTax;
		std::vector<std::vector<int>> jobInpblyMachime;
		std::vector<std::vector<int>> jobPblyMachime;
		std::vector<std::vector<int>> machimePblyJob;
		std::vector<int> numTools;
	
		std::map<std::pair<int,int>, int> difJob;
		std::deque<double> allTemps;
	public:
		PT(float tempMin, float tempMax, int tempL, int MKL, int PTL, int tempUp, int tempD, int stopC); // constructor
		~PT();
		void start(int nt, int mf, int ut, int thN, bool tft);
		void stop();
		void calcBest();
		void printResult();
		void printBestSol();
		void generateFiles();
		void loadInstance(std::string filename);
		std::deque<double> tempPG(float tempMin, float tempMax, int tempL); 
		std::deque<double> tempExp(float tempMin, float tempMax, int tempL); 
		std::deque<double> tempLinear(float tempMin, float tempMax, int tempL); 
		std::deque<double> tempILinear(float tempMin, float tempMax, int tempL); 
};

#endif
