/*
 * Problem.cpp
 * 
 * Copyright 2021 André Luis <André Luis@DESKTOP-HDL2CBS>
 * 
 * 
 */

#include "../include/NPM.h"

thread_local std::vector<std::pair <int,int>> NPM::twoSwap;

NPM::NPM(int numberJobs_, int numberTools_, std::vector<int> capacityMagazine_,std::vector<std::vector<bool>>  toolJob_, std::vector<std::vector<int>>  JobTools_, std::map<std::pair<int,int>, int> difJob_, int sum_, int numberMachine_, std::vector<int> changeTax_, std::vector<std::vector<int>> JobTax_, std::vector<std::vector<int>> jobInpblyMachime_, std::vector<int> numTools_,std::vector<std::vector<int>> machimePblyJob_, std::vector<std::vector<int>> jobPblyMachime_, bool tft_)
	:numberJobs(numberJobs_)
	,numberTools(numberTools_)
	,numberMachine(numberMachine_)
	,changeTax(changeTax_)
	,moveF(1)
	,capacityMagazine(capacityMagazine_)
	,meio(numberJobs_/2)
	,sum(sum_)
	,toolJob(toolJob_)
	,JobTools(JobTools_)
	,difJob(difJob_)
	,JobTax(JobTax_)
	,jobInpblyMachime(jobInpblyMachime_)
	,numTools(numTools_)
	,machimePblyJob(machimePblyJob_)
	,jobPblyMachime(jobPblyMachime_)
	,tft(tft_)
	{}
NPM::~NPM(){
	
}

void NPM::setNeighborType(int nType){
	nType_= nType;
}

void NPM::setMoveForce(int nF){
	moveF = nF;
}


std::pair<int, int> NPM::KTNS5(std::vector<int> sol, int Mach){
	
	int evalSol = 0; 
	int custo = 0;
	int flowtime = 0;
	int jL;
	int jobSize = sol.size();
	std::vector<bool> magazineL(numberTools, true);

	for(jL= 0; jL < jobSize; ++jL){
		std::vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;
		int changePos = 0;
		custo += JobTax[Mach][sol[jL]];
		
		while((cmL < capacityMagazine[Mach]) && (left < jobSize)){
			for (auto it=JobTools[sol[left]].begin(); ((it!=JobTools[sol[left]].end()) && (cmL < capacityMagazine[Mach])); ++it){
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					//++trocaCL;
					++evalSol;
					++cmL;
					++changePos;
				}
			}
			++left;
		}
		
		for(int t=0; ((t < numberTools) && (cmL < capacityMagazine[Mach])); t++){
			if((magazineL[t]) && (!magazineCL[t])){
				magazineCL[t] = true;
				++cmL;			
			}
		}
		
		magazineL = magazineCL;
		custo += changeTax[Mach] * changePos;
		flowtime += custo;
	//	std::cout<<"Job: "<<sol[jL]<<" custo: "<<custo<<" flowtime: "<<flowtime<<"\n";
	}

//	return evalSol;
	return std::make_pair(flowtime, evalSol);

}


int NPM::GPCA(std::vector<int> sol, int capM){
	
	int pipes_count = 0;
	int somaUm = 0;
	int last_full = 0;
	int jobSize = sol.size();
	
	std::vector<int> last_seen(numberTools);
	std::vector<std::vector<int>> M;
	

	//Completa o last_seen
	for(int i = 0; i < numberTools; ++i){
		if(toolJob[i][sol[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[sol[0]]);
	somaUm += JobTools[sol[0]].size();
	
	
	for(int e = 1; e < jobSize; ++e){
		somaUm += JobTools[sol[e]].size();

		M.push_back(JobTools[sol[e]]);
				
		for (auto t = JobTools[sol[e]].begin(); t != JobTools[sol[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				
				
				for(int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if((int)M[i].size() == capM) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if((int)M[e].size() == capM) last_full = e;

	}
	
  int trocas = (somaUm - capM - pipes_count);
  if(trocas < 0) trocas = 0;

 return trocas;	

}

std::pair<int, int> NPM::GPCANPM(std::vector<int> sol, int Mach){

	int pipes_count = 0;
	int custo = 0;
	int somaUm = 0;
	int last_full = 0;
	int jobSize = sol.size();
	
	std::vector<int> last_seen(numberTools);
	std::vector<std::vector<int>> M;
	

	//Completa o last_seen
	for(int i = 0; i < numberTools; ++i){
		if(toolJob[i][sol[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[sol[0]]);
	custo += JobTax[Mach][sol[0]];
	somaUm += JobTools[sol[0]].size();
	
	
	for(int e = 1; e < jobSize; ++e){
		custo += JobTax[Mach][sol[e]];
		somaUm += JobTools[sol[e]].size();

		M.push_back(JobTools[sol[e]]);
				
		for (auto t = JobTools[sol[e]].begin(); t != JobTools[sol[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
								
				for(int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if((int)M[i].size() == capacityMagazine[Mach]) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if((int)M[e].size() == capacityMagazine[Mach]) last_full = e;

	}
	
 int trocas = (somaUm - capacityMagazine[Mach] - pipes_count);
 if	(trocas < 0) trocas = 0;
 return std::make_pair((custo+(changeTax[Mach] * trocas)),trocas); 
}

solution NPM::construction(){
	solution ss;
	std::vector<int> jobs(numberJobs,-1);
    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()}; 
	int taxaini = numberJobs/numberMachine;

	for(int np = 0; np < numberMachine; ++np){
		int taxa = taxaini;
		
		if(machimePblyJob[np].size() < taxaini){
			taxa = machimePblyJob[np].size();
		}

		int start = 0;
		int stop = taxa;
				
		if(np > 0){
			start = ss.cutEnd[np -1];
			stop = ss.cutEnd[np -1] + taxa;		
		}
				
		if(np == (numberMachine -1)) stop = numberJobs; 
		
		ss.cutStart.push_back(start);
		ss.cutEnd.push_back(stop);
	}
	
	std::vector<int> posMachine(numberMachine, 0);
	std::vector<std::vector<int>> jPM = jobPblyMachime;
	std::sort(jPM.begin(),jPM.end(), [](const std::vector<int> & a, const std::vector<int> & b){ return a.size() < b.size(); });

	
	for (int critJob = 0; critJob < jPM.size(); critJob++){
		int job = jPM[critJob][0];
		std::shuffle(jPM[critJob].begin()+1, jPM[critJob].end(), mersenne_engine);
		bool ok = false;
		bool ok2 = false;
		bool ok3 = false;

		for(int mach = 1; mach < jPM[critJob].size();){
			
			if(posMachine[jPM[critJob][mach]] < (ss.cutEnd[jPM[critJob][mach]] - ss.cutStart[jPM[critJob][mach]])){
				if(jobs[posMachine[jPM[critJob][mach]]+ ss.cutStart[jPM[critJob][mach]]] == -1){
					jobs[posMachine[jPM[critJob][mach]]+ ss.cutStart[jPM[critJob][mach]]] = job;
					posMachine[jPM[critJob][mach]]++;
					mach = jPM[critJob].size();
					ok = true;
				}else{mach++; std::cout<<"\n !!!!!!!!!!!! ATENCAO  NAO É NEGATIVO !!!!!!!!!!!! \n";}
			}else{
				mach++; 

				}
		}
		
		
		if(!ok){
			
			for(int mach = 1; mach < jPM[critJob].size();){
				for(int sss = ss.cutStart[jPM[critJob][mach]]; sss < ss.cutEnd[jPM[critJob][mach]];){
					int jobs2  = jobs[sss];
					
					for(int mach2 = 1; mach2 < jobPblyMachime[jobs2].size();){						
						if(posMachine[jobPblyMachime[jobs2][mach2]] < (ss.cutEnd[jobPblyMachime[jobs2][mach2]] - ss.cutStart[jobPblyMachime[jobs2][mach2]])){
							if(jobs[posMachine[jobPblyMachime[jobs2][mach2]] + ss.cutStart[jobPblyMachime[jobs2][mach2]]] == -1){
								
								
								jobs[posMachine[jobPblyMachime[jobs2][mach2]] + ss.cutStart[jobPblyMachime[jobs2][mach2]]] = jobs2;
								jobs[sss] = job;
								posMachine[jobPblyMachime[jobs2][mach2]]++;
							
								//termina busca
								mach2 = jobPblyMachime[jobs2].size();
								sss = ss.cutEnd[jPM[critJob][mach]];
								mach = jPM[critJob].size();
								ok2 = true;	
								
							} else{
								mach2++; std::cout<<"\n !!!!!!!!!!!! ATENCAO  NAO É NEGATIVO 2 !!!!!!!!!!!! \n";
								}					
						}else{
							mach2++;
							}
					}
				 sss++;
				}
				mach++;
			}
		
		}
		
		
		if(!ok && !ok2){
			
			for(int mach = 1; mach < jPM[critJob].size();){
				if((ss.cutEnd[jPM[critJob][mach] -1] - ss.cutStart[jPM[critJob][mach] -1]) > 1){
					if(jobs[ss.cutStart[jPM[critJob][mach]] - 1] == -1){
						jobs[ss.cutStart[jPM[critJob][mach]] - 1] =  job;
						posMachine[jPM[critJob][mach]]++;
						ss.cutEnd[jPM[critJob][mach] - 1]--;
						ss.cutStart[jPM[critJob][mach]]--;
					
						ok3 = true;	
						mach = jPM[critJob].size();
									
					}else{
						mach++;
					}
				}else{
					mach++;
				}
			}	
		}
		
		if(!ok && !ok2 && !ok3){
			std::cout<<"\n !!!!!!!!!!!! ATENCAO ERRO EM REALOCAR JOB Tamanho máquina !!!!!!!!!!!! Job: "<<job<<"\n";		
		}
		
		
	}
	
	for(int np = 0; np < numberMachine; ++np){
			std::shuffle(jobs.begin() + ss.cutStart[np], jobs.begin() + ss.cutEnd[np], mersenne_engine);
	}
	
	
ss.sol = jobs;
		
	for(int np = 0; np < numberMachine; ++np){
		for(int start = ss.cutStart[np]; start < ss.cutEnd[np]; start++){
		if(JobTools[ss.sol[start]].size() > capacityMagazine[np]){
		 std::cout<<"\n Problema capacidade \n";
		 std::cout<<"tarefa: "<<ss.sol[start]<<" ferramentas: "<<JobTools[ss.sol[start]].size()<<" capacidade: "<<capacityMagazine[np];
		}
		
		}		
	}

	
	
	

	setCut(&ss);
	int erro = validateSol(0, ss.sol.size(), ss);
	if(erro > 1) std::cout <<"\n Erro cap: "<<erro<< std::endl;

	ss = oneBlock(ss);
				
	return ss;

}

solution NPM::neighbor(solution sol){
	switch (nType_)
	{
		case 1:{ 
			return neighbor2opt(sol); 
		}
		break;
		case 2:{ 
			return neighbor2Swap(sol); 
		}
		break;
		case 3:{ 
			return neighborRandIns(sol); 
		}
		break;
		case 4:{ 
			return neighbor2optCM(sol); 
		}
		break;
		case 5:{ 
			return neighbor2SwapCM(sol); 
		}
		break;
		case 6:{ 
			return neighborRandInsCM(sol); 
		}
		break;
		case 7:{
			std::random_device rnd_device;
			std::mt19937 mersenne_engine {rnd_device()};
			std::uniform_real_distribution<> dist {0.0, 1.0}; 
			if(dist(mersenne_engine) < 0.5){
				return neighbor2optCM(sol); 				
			}else{
				return neighbor2SwapCM(sol); 
			}
		}
		break;
		case 8:{
			std::random_device rnd_device;
			std::mt19937 mersenne_engine {rnd_device()};
			std::uniform_real_distribution<> dist {0.0, 1.0}; 
			if(dist(mersenne_engine) < 0.5){
				return neighbor2opt(sol); 				
			}else{
				return neighbor2Swap(sol); 
			}
		}
		break;
		default:
			return neighbor2opt(sol); 
		break;	
	}
}

int NPM::machine(int index, solution sol){
	int machine = -1;
	do {
		++machine;
	} while (!((index >= sol.cutStart[machine]) && (index < sol.cutEnd[machine])));
	return machine;
}

int NPM::validateSol(int start, int end, solution sol){
	
//	std::cout<<"################## Validade ##################\n";
	int erro = 1;
	int machine = -1;
	bool valid = true;
	do {
		++machine;
	} while (!((start >= sol.cutStart[machine]) && (start < sol.cutEnd[machine])));
	
	while(start < end){
	//	std::cout<<"machine: "<<machine<<"\n";
		
		if(JobTools[sol.sol[start]].size() > capacityMagazine[machine]){
			valid = false;
			// start = end;
			erro++;
		//	std::cout<<"validade Erro job: "<<sol.sol[start]<<" quant ferr: "<<JobTools[sol.sol[start]].size()<<" capacidade: "<<capacityMagazine[machine]<<"\n";
		}
		
		++start;
		if(start >= sol.cutEnd[machine])++machine;
	}

return erro;
}

int NPM::validateSol(int machine, std::vector<int> sol){
	
//	std::cout<<"################## Validade ##################\n";
	int erro = 1;
	int start = 0;
	int end = sol.size();
	
	while(start < end){
	//	std::cout<<"machine: "<<machine<<"\n";
		
		if(JobTools[sol[start]].size() > capacityMagazine[machine]){
			erro++;
	//		std::cout<<"Erro de capacidade job: "<<sol.sol[start]<<" quant ferr: "<<JobTools[sol.sol[start]].size()<<" capacidade: "<<capacityMagazine[machine]<<"\n";
		}
		
		++start;
	}

return erro;
}


int NPM::fixSol(int start, int end, solution* sol){
	
//	std::cout<<"################## Validade ##################\n";
	int erro = 1;
	int machine = 0;
	bool valid = true;
	
	while(start < end){
	//	std::cout<<"machine: "<<machine<<"\n";
		
		if(JobTools[sol->sol[start]].size() > capacityMagazine[machine]){
			valid = false;
			// start = end;
			erro++;
			std::cout<<"Erro de capacidade job: "<<sol->sol[start]<<" quant ferr: "<<JobTools[sol->sol[start]].size()<<" capacidade: "<<capacityMagazine[machine]<<"\n";
			int m = 0;
			
			for(int s = 0; s < end; s++){
				if((JobTools[sol->sol[start]].size() < capacityMagazine[m]) && (JobTools[sol->sol[s]].size() < capacityMagazine[machine])){
					std::cout<<"troca por: "<<sol->sol[s]<<" quant ferr: "<<JobTools[sol->sol[s]].size()<<" capacidade: "<<capacityMagazine[m]<<"\n";
					std::swap(sol->sol[s], sol->sol[start]);
					s = end;
					erro--;					
				}else{
					if(s >= sol->cutEnd[m])++m;
				}
			}
		
		}
		
		++start;
		if(start >= sol->cutEnd[machine])++machine;
	}

return erro;
}



solution NPM::neighborRandIns(solution sol){
	
	
	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;
	s.cm = sol.cm;
	
	int first = 0;
	int last = 0;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};
			
	for(int i = 0 ; i < moveF ; i++){

		do {		
			first = dist(mersenne_engine);
			last = dist(mersenne_engine);		
		} while(first == last);
		
		int mfirst = machine(first, s);
		int mlast = machine(last, s);
		
		auto it1 = std::find(jobInpblyMachime[s.sol[first]].begin(), jobInpblyMachime[s.sol[first]].end(), mlast);

		if ((it1 == jobInpblyMachime[s.sol[first]].end()) && ((s.cutEnd[mfirst] - s.cutStart[mfirst]) >1)){
			int value = s.sol[first];
			s.sol.erase(s.sol.begin()+first);
	
			if(last != (numberJobs-1)){
				s.sol.insert(s.sol.begin()+last, value);
			}else{
				s.sol.insert(s.sol.end(), value);	
			}
			
			while(mfirst < mlast){
				--s.cutEnd[mfirst];
				--s.cutStart[mfirst+1];
				mfirst++;
			}			
		}
	} // END FORCE
		
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;

}

solution NPM::neighborRandInsCM(solution sol){
	
	
	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;
	s.cm = sol.cm;
	
	int first = 0;
	int last = 0;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> distCM{sol.cutStart[sol.cm], (sol.cutEnd[sol.cm]-1)};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};
			
	for(int i = 0 ; i < moveF ; i++){

		do {		
			first = distCM(mersenne_engine);
			last = dist(mersenne_engine);		
		} while(first == last);
		
		int mfirst = machine(first, s);
		int mlast = machine(last, s);
		
		auto it1 = std::find(jobInpblyMachime[s.sol[first]].begin(), jobInpblyMachime[s.sol[first]].end(), mlast);

		if ((it1 == jobInpblyMachime[s.sol[first]].end()) && ((s.cutEnd[mfirst] - s.cutStart[mfirst]) >1)){
			int value = s.sol[first];
			s.sol.erase(s.sol.begin()+first);
	
			if(last != (numberJobs-1)){
				s.sol.insert(s.sol.begin()+last, value);
			}else{
				s.sol.insert(s.sol.end(), value);	
			}
			
			while(mfirst < mlast){
				--s.cutEnd[mfirst];
				--s.cutStart[mfirst+1];
				mfirst++;
			}			
		}
	} // END FORCE
		
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;

}


solution NPM::neighbor2opt(solution sol){

	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;
	int first = 0;
	int last = 0;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};
	
	for(int i = 0 ; i < moveF ; i++){
		do {		
			first = dist(mersenne_engine);
			last = dist(mersenne_engine);		
		} while(first == last);
	
		if (first > last)std::swap(first,last);
	
		while (first < last) {
			int mfirst = machine(first, s);
			int mlast = machine(last, s);
			
			auto it1 = std::find(jobInpblyMachime[s.sol[last]].begin(), jobInpblyMachime[s.sol[last]].end(), mfirst);
			auto it2 = std::find(jobInpblyMachime[s.sol[first]].begin(), jobInpblyMachime[s.sol[first]].end(), mlast);
			
			if ((it1 == jobInpblyMachime[s.sol[last]].end()) && ((it2 == jobInpblyMachime[s.sol[first]].end()))){
				std::swap(s.sol[first], s.sol[last]);
			}
			
			first++;
			last--;
		}
	}
		
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;
}

solution NPM::neighbor2optCM(solution sol){

	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;
	int first = 0;
	int last = 0;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> distCM{sol.cutStart[sol.cm], (sol.cutEnd[sol.cm]-1)};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};
	
	for(int i = 0 ; i < moveF ; i++){
		do {		
			first = distCM(mersenne_engine);
			last = dist(mersenne_engine);		
		} while(first == last);
	
		if (first > last)std::swap(first,last);
	
		while (first < last) {
			int mfirst = machine(first, s);
			int mlast = machine(last, s);
			
			auto it1 = std::find(jobInpblyMachime[s.sol[last]].begin(), jobInpblyMachime[s.sol[last]].end(), mfirst);
			auto it2 = std::find(jobInpblyMachime[s.sol[first]].begin(), jobInpblyMachime[s.sol[first]].end(), mlast);
			
			if ((it1 == jobInpblyMachime[s.sol[last]].end()) && ((it2 == jobInpblyMachime[s.sol[first]].end()))){
				std::swap(s.sol[first], s.sol[last]);
			}
			
			first++;
			last--;
		}
	}
		
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;
}

solution NPM::neighbor2Swap(solution sol){

	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;

	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};

	
	if (twoSwap.size()==0){
		for(int pos1 = 0; pos1 < numberJobs; pos1++){		
			for(int pos2 = (pos1+1); pos2 < numberJobs; pos2++){		
				twoSwap.push_back(std::make_pair (pos1,pos2));
			}
		}
	}
	


	std::shuffle(begin(twoSwap), end(twoSwap), mersenne_engine);
	
	
	for(int i = 0;i < moveF; i++){
		
		int mfirst = machine(twoSwap[i].first, s);
		int mlast = machine(twoSwap[i].second, s);
		
		auto it1 = std::find(jobInpblyMachime[s.sol[twoSwap[i].first]].begin(), jobInpblyMachime[s.sol[twoSwap[i].first]].end(), mlast);
		auto it2 = std::find(jobInpblyMachime[s.sol[twoSwap[i].second]].begin(), jobInpblyMachime[s.sol[twoSwap[i].second]].end(), mfirst);

		if ((it1 == jobInpblyMachime[s.sol[twoSwap[i].first]].end()) && ((it2 == jobInpblyMachime[s.sol[twoSwap[i].second]].end()))){			
			std::swap(s.sol[twoSwap[i].first], s.sol[twoSwap[i].second]);
		}
	}
	
	
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;

}


solution NPM::neighbor2SwapCM(solution sol){

	solution s;
	s.sol = sol.sol;
	s.cutEnd = sol.cutEnd;
	s.cutStart = sol.cutStart;
	int first = 0;
	int last = 0;

	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> distCM{sol.cutStart[sol.cm], (sol.cutEnd[sol.cm]-1)};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};

		
	
	for(int i = 0;i < moveF; i++){
		
		do {		
			first = distCM(mersenne_engine);
			last = dist(mersenne_engine);		
		} while(first == last);

		
		int mfirst = machine(first, s);
		int mlast = machine(last, s);
		
		auto it1 = std::find(jobInpblyMachime[s.sol[first]].begin(), jobInpblyMachime[s.sol[first]].end(), mlast);
		auto it2 = std::find(jobInpblyMachime[s.sol[last]].begin(), jobInpblyMachime[s.sol[last]].end(), mfirst);

		if ((it1 == jobInpblyMachime[s.sol[first]].end()) && ((it2 == jobInpblyMachime[s.sol[last]].end()))){			
			std::swap(s.sol[first], s.sol[last]);
		}
	}
	
	
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;
				
	return s;
}



int NPM::evaluate(solution *sol){

int trocas = 0;
	for(int np = 0; np < numberMachine; ++np){
		std::vector<int> subsol = {sol->sol.begin() + sol->cutStart[np], sol->sol.begin() +  sol->cutEnd[np]};
		trocas += GPCA(sol->sol,capacityMagazine[np]);
	}
return trocas;
}

std::pair<int, int> NPM::makespanAndChange(solution* sol){
	solution s = *sol;
	int make = 0;
	int change = 0;
		
	for(int i = 0; i < numberMachine; ++i){
		std::vector<int> sv = std::vector<int>(s.sol.begin() + s.cutStart[i], s.sol.begin() + s.cutEnd[i]);		
			std::pair<int, int> tax = GPCANPM(sv, i);
			if(tax.first > make) make = tax.first;
			change += tax.second;
	}
return std::make_pair(make, change);
}


solution NPM::oneBlock(solution sol){
	
	solution s = sol;
	solution best = sol;
	
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()}; 
	std::vector<int> ferramentas;
	for (int j = 0; j < numberTools; ++j) {
		ferramentas.push_back(j);
	}
	
	std::shuffle(ferramentas.begin(), ferramentas.end(), mersenne_engine);

	for(int i = 0; i < numberMachine; ++i){
		for (int f : ferramentas) {
			int lastSee = -1;
			for(int j = s.cutStart[i]; j < (s.cutEnd[i]-1); ++j){
			
				if((toolJob[f][s.sol[j]] == 1) && (toolJob[f][s.sol[j+1]] == 0)){
					lastSee = j;
				}
				
				if((lastSee != -1) && (toolJob[f][s.sol[j]] == 0) && (toolJob[f][s.sol[j+1]] == 1)){
					int value = s.sol[j+1];
					s.sol.erase(s.sol.begin()+(j+1));	
					s.sol.insert(s.sol.begin()+(lastSee+1), value);
					setCut(&s);
					if(best.evalSol > s.evalSol){
						j = lastSee;
						lastSee = -1;
						best = s;
					}else{
						s = best;
					}
				}
			
			}
		}
	}
 return best;
}

void NPM::setCut(solution* sol){
	
	solution s = *sol;
	solution best = *sol;
	best.evalSol = INT_MAX; 

	int tftM = 0;
	int max = 0;
	int maxChange = 0;
	int cm = -1;
	bool exit = false;
	
	while((cm < (numberMachine-1)) and !(exit)){
		max = 0;
		maxChange = 0;
		cm = -1;
		
		
		for(int i = 0; i < numberMachine; ++i){
			std::vector<int> sv = std::vector<int>(s.sol.begin() + s.cutStart[i], s.sol.begin() + s.cutEnd[i]);		
			if(!tft){
				std::pair<int, int> tax = GPCANPM(sv, i);
				if(tax.first > max){
					max = tax.first;
					cm = i;
				}
			}else{
				std::pair<int, int> tax = KTNS5(sv, i);
				max+= tax.first;
			cm = tftM;
			}
		}
	
		//Melhorou 
		if(best.evalSol > max){
			s.evalSol = max;
			s.cm = cm;
			best = s;
		}else if(tft){
			++tftM;
			cm = tftM;
			s = best;
		}
		
		if((s.cutEnd[cm] - s.cutStart[cm]) <= 0 ){
			++tftM;
			cm = tftM;
		}
		
		if((cm+1) < numberMachine){
			int from = cm;
			int elePos = (s.cutEnd[cm] -1);
			int ele = s.sol[elePos];

			auto it = std::upper_bound((jobPblyMachime[ele].begin()+1), jobPblyMachime[ele].end(), cm);			
			if (it != jobPblyMachime[ele].end()){
				while(from < *it){
					
					--s.cutEnd[from]; 
					--s.cutStart[from+1];
					
					if(((from+1) == *it) && (cm != from)){
						s.sol.erase(s.sol.begin()+elePos);
						s.sol.insert(s.sol.begin()+s.cutStart[from+1], ele);
					}
																				
					from++;
				}
			}else{				
				exit = true;
				}
		}
	}
	
		s = best;
		exit = false;
		tftM = (numberMachine-1);
		
		while((cm > 0) and !(exit)){
		max = 0;
		maxChange = 0;
		cm = -1;
		
		for(int i = 0; i < numberMachine; ++i){
			std::vector<int> sv = std::vector<int>(s.sol.begin() + s.cutStart[i], s.sol.begin() + s.cutEnd[i]);		

			if(!tft){
				std::pair<int, int> tax = GPCANPM(sv, i);	
				if(tax.first > max){
					max = tax.first;
					cm = i;
				}
			}else{
				std::pair<int, int> tax = KTNS5(sv, i);

				max+= tax.first;
			cm = tftM;
			}
		}
	
		if(best.evalSol >= max){
			s.evalSol = max;
			s.cm = cm;
			best = s;
		}else if(tft){
			--tftM;
			cm = tftM;
			s = best;
		}
		
		if((s.cutEnd[cm] - s.cutStart[cm]) <= 0 ){
			--tftM;
			cm = tftM;
		}

		
		if((cm-1) >= 0){
			
			int from = cm;
			int elePos = s.cutStart[cm];				
			int ele = s.sol[elePos];

			auto it = std::find((jobPblyMachime[ele].begin()+1), jobPblyMachime[ele].end(), cm);
			int dist = std::distance(jobPblyMachime[ele].begin(),it);

			if (dist > 1){
				int mach = jobPblyMachime[ele][dist-1];
				while(from > mach){

					++s.cutEnd[from-1]; 
					++s.cutStart[from];

					if(((from-1) == mach) && (cm != from)){
						s.sol.erase(s.sol.begin()+elePos);
						s.sol.insert((s.sol.begin()+s.cutEnd[from-1]-1), ele);					
					}	
									
					from--;
				}
			}else{				
				exit = true;
				}
					
		}
		
	}

	*sol = best;
}




int NPM::getNumberJobs(){
	return numberJobs;
}

int NPM::getNumberTools(){
	return numberTools;
}

std::vector<int> NPM::getCapacityMagazine(){
	return capacityMagazine;
}

std::vector<std::vector<bool>>* NPM::getToolJob(){
	return &toolJob;
}

std::vector<std::vector<int>>* NPM::getJobTools(){
	return &JobTools;
}


