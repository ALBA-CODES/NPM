#ifndef __NPM_H__
#define __NPM_H__

#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <utility>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <omp.h>
#include <random>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <thread>
#include <atomic>
#include "Problem.h"


class NPM: public Problem{
	private:
		int numberJobs;
		int numberTools;
		int numberMachine;
		std::vector<int> changeTax;
		int nType_;
		int moveF;
		std::vector<int> capacityMagazine;
		int meio;
		int sum;
		std::vector<std::vector<bool>>  toolJob;
		std::vector<std::vector<int>>  JobTools;
		std::map<std::pair<int,int>, int> difJob;
		std::vector<std::vector<int>> JobTax;
		std::vector<std::vector<int>> jobInpblyMachime;
		std::vector<std::vector<int>> jobPblyMachime;
		std::vector<std::vector<int>> machimePblyJob;
		std::vector<int> numTools;
		bool tft;

		thread_local static std::vector<std::pair <int,int>> twoSwap;

	public:
		NPM(int numberJobs_, int numberTools_, std::vector<int> capacityMagazine_,std::vector<std::vector<bool>>  toolJob_, std::vector<std::vector<int>>  JobTools_, std::map<std::pair<int,int>, int> difJob_, int sum_, int numberMachine_, std::vector<int> changeTax_, std::vector<std::vector<int>> JobTax_, std::vector<std::vector<int>> jobInpblyMachime_, std::vector<int> numTools_,std::vector<std::vector<int>> machimePblyJob_, std::vector<std::vector<int>> jobPblyMachime_, bool tft_);
		~NPM();
		void setNeighborType(int nType);
		void setMoveForce(int mF);
		std::pair<int, int> KTNS5(std::vector<int> sol, int Mach);
		int GPCA(std::vector<int> sol, int capM);
		std::pair<int, int> GPCANPM(std::vector<int> sol, int Mach);
		void setCut(solution* sol);


		solution construction();
		solution oneBlock(solution sol);
		solution neighbor(solution sol);
		solution neighbor2opt(solution sol);
		solution neighbor2Swap(solution sol);
		solution neighborRandIns(solution sol);
		solution neighbor2optCM(solution sol);
		solution neighbor2SwapCM(solution sol);
		solution neighborRandInsCM(solution sol);
		solution localSearch(solution sol);
		solution localSearch2Swap(solution sol);
		solution localSearch2opt(solution sol);
		solution localSearchRandIns(solution sol);
		int validateSol(int start, int end,solution sol);
		int validateSol(int machine, std::vector<int> sol);
		int fixSol(int start, int end,solution* sol);
		int evaluate(solution* sol);
		int getNumberJobs();
		int getNumberTools();
		std::vector<int> getCapacityMagazine();
		int calcBestJoin(solution* neigh);
		std::vector<std::vector<bool>>*  getToolJob();
		std::vector<std::vector<int>>* getJobTools();
		int machine(int index, solution sol);
		std::pair<int, int> makespanAndChange(solution* sol);

};

#endif 
