/*
 * PT.cpp
 * 
 * Copyright 2021 André Luis <André Luis@DESKTOP-HDL2CBS>
 *  
 * 
 */

#include "../include/PT.h"
 
// inicialização do PT. valores da temp. tamanho da MCMC e o Número de propostas de troca entre temp
PT::PT(float tempMin, float tempMax, int tempL, int MKL, int PTL, int tempUp, int tempD, int stopC)
	: tempMin_(tempMin)
	, tempMax_(tempMax)
	, tempL_(tempL)
	, MKL_(MKL)
	, PTL_(PTL)
	, tempUpdate(tempUp)
{
	if(stopC == 0) stopC_ = 0.1 * PTL;
	else stopC_ = stopC;	
	
	switch (tempD){
		case 1:{
			allTemps = tempLinear(tempMin,tempMax,tempL);
		}
		break;
		case 2:{
			allTemps = tempILinear(tempMin,tempMax,tempL);
		}
		break;
		case 3:{
			allTemps = tempExp(tempMin,tempMax,tempL);
		}
		break;
		case 4:{
			allTemps = tempPG(tempMin,tempMax,tempL);
		}
		break;
		default:
			cout << "Invalid initial temperature distribution! \n";
		break;	
	}

} 

PT::~PT(){

}

void PT::start(int nt, int mf, int ut, int thN, bool tft){
	ExecTime et;
	Consumer* consumer = new Consumer(thN, stopC_, &et);
	atomic<int> PTLEnd = PTL_;

	//variaveis
	Node* nMCMC;
	Node* nSwap;
	Node* nUpTemp;	
	Node* nMCMCAux;
	Node* nSwapAux;
	Node* nUpTempAux;
		    
    // Cria o primeiro nó MCMC
    NPM* prob_= new NPM(numberJobs,numberTools,capacityMagazine,toolJob,JobTools, difJob, sum, numberMachine, changeTax, JobTax, jobInpblyMachime, numTools, machimePblyJob, jobPblyMachime, tft);
    prob_->setNeighborType(nt);	
    prob_->setMoveForce(mf);
    thN-=2;	
	nMCMC = new NodeMCMC(MKL_,&PTLEnd,allTemps.front(), prob_, consumer, (thN>1));
	((NodeMCMC*)nMCMC)->setFirstTemp(); // check First temp 
	consumer->setMaxEnd();
	allTemps.pop_front();
	nMCMCAux = nMCMC; 
	
		
    // Cria o segundo nó MCMC
    prob_= new NPM(numberJobs,numberTools,capacityMagazine,toolJob,JobTools, difJob, sum, numberMachine, changeTax, JobTax, jobInpblyMachime, numTools, machimePblyJob, jobPblyMachime, tft);	
    prob_->setNeighborType(nt);	
    prob_->setMoveForce(mf);
    thN-=2;	
	nMCMC = new NodeMCMC(MKL_,&PTLEnd,allTemps.front(), prob_, consumer, (thN>1));
	consumer->setMaxEnd();
	allTemps.pop_front();
	
	
	// Cria o primeiro nó de troca
	if(!ut) nSwap = new NodeSwap(nMCMC,nMCMCAux, consumer, &PTLEnd);
	else{ 
		nUpTemp = new NodeTempUp(nMCMC,nMCMCAux, NULL, consumer,ut, tempL_);
		nSwap = new NodeSwap(nMCMC,nMCMCAux,nUpTemp, consumer, &PTLEnd, tempUpdate);	
	}
	
	consumer->setMaxEnd();
	

	// arestas chegando no swap	
	nSwap->addEdge(nMCMCAux,nSwap);
	nSwap->addEdge(nMCMC,nSwap);
	
	// arestas saindo do swap
	nSwap->addEdge(nSwap, nMCMCAux);
	nSwap->addEdge(nSwap, nMCMC);
	
	// atualiza as arestas
	nMCMCAux->addEdge(nMCMCAux,nSwap);
	nMCMC->addEdge(nMCMC,nSwap);
	nMCMCAux->addEdge(nSwap,nMCMCAux);
	nMCMC->addEdge(nSwap,nMCMC);

	
	// comeca a execucao	
	consumer->execAsync(nMCMCAux);

	// recupera o nó a direita
	nMCMCAux = nMCMC;
	nSwapAux = nSwap;
	nUpTempAux = nUpTemp;
	
	// Cria os nós restantes	
	while(!allTemps.empty()){

		prob_= new NPM(numberJobs,numberTools,capacityMagazine,toolJob,JobTools, difJob, sum, numberMachine, changeTax, JobTax, jobInpblyMachime, numTools, machimePblyJob, jobPblyMachime, tft);

		prob_->setNeighborType(nt);
		prob_->setMoveForce(mf);
		thN-=2;	
		nMCMC = new NodeMCMC(MKL_,&PTLEnd, allTemps.front(),prob_, consumer, (thN>1));
		consumer->setMaxEnd();
		allTemps.pop_front();

		// cria o nó de troca
		if(!ut) nSwap = new NodeSwap(nMCMC,nMCMCAux,consumer,&PTLEnd);
		else {
			nUpTemp = new NodeTempUp(nMCMC,nMCMCAux, nUpTempAux, consumer,ut, tempL_);
			((NodeTempUp*)nUpTempAux)->setUpTempUp(nUpTemp);
			nSwap = new NodeSwap(nMCMC,nMCMCAux,nUpTemp,consumer, &PTLEnd, tempUpdate);
		}
		
		consumer->setMaxEnd();
		
		// Atualiza as arestas que chegam
		nSwap->addEdge(nMCMCAux,nSwap);
		nSwap->addEdge(nMCMC,nSwap);
		nSwap->addEdge(nSwapAux,nSwap);
		nMCMCAux->addEdge(nSwap, nMCMCAux);
		nMCMC->addEdge(nSwap,nMCMC);		

		//atualiza as arestas saem 
		nMCMCAux->addEdge(nMCMCAux,nSwap);
		nMCMC->addEdge(nMCMC,nSwap);			
		nSwapAux->addEdge(nSwapAux,nSwap);
		nSwap->addEdge(nSwap, nMCMCAux);
		nSwap->addEdge(nSwap, nMCMC);

		//start exec
		consumer->execAsync(nMCMCAux);
		
		//update varibles
		nMCMCAux = nMCMC;
		nSwapAux = nSwap;
		nUpTempAux = nUpTemp;
	}

	//check last temp
	((NodeMCMC*)nMCMCAux)->setLastTemp();
	if(ut)((NodeTempUp*)nUpTempAux)->setUpTempUp(NULL);
	//start exec
	consumer->execAsync(nMCMCAux);


consumer->finished();
solution best = consumer->getBestSol();
int erro = prob_->validateSol(0, best.sol.size(), best);
std::pair<int, int> MandC = prob_->makespanAndChange(&best);
//prob_->setCut(&best);

//solution best2 = prob_->oneBlock(best);
//int erro2 = prob_->validateSol(0, best2.sol.size(), best2);

//cout<<"Best sol: "<<best.evalSol<<" Erro: "<<(erro-1)<<" Best2 sol: "<<best2.evalSol<<" Erro2: "<<(erro2-1)<<"\n";


et.getTime(tempMin_,tempMax_,tempL_,MKL_,PTL_,fileName,best,numberMachine, consumer->getIndexPT(), (erro-1), MandC.first, MandC.second);

//cout<<best.evalSol;
printf("%d.%07d", best.evalSol, et.getDuration());
//cout<<"Best sol: "<<best.evalSol<<" PTL best sol: "<<*consumer->getIndexPT()<<" Erro: "<<(erro-1)<<"\n";
}


std::deque<double> PT::tempPG(float tempMin, float tempMax, int tempL){
	// variaveis
	std::deque<double> temps(tempL);
	double b = tempMax/tempMin;
	double ex = 1.0/((double)(tempL - 1));	
	double r =  std::pow(b,ex); 
	// Calculando as temp
	for(unsigned int ind = 0; ind < temps.size();++ind){
		temps[ind] = tempMin * std::pow(r,ind);	
	}

	return temps;
}
std::deque<double> PT::tempExp(float tempMin, float tempMax, int tempL){
	// variaveis
	std::deque<double>temps(tempL);
	double tempMaxExp = log(tempMax);
	double tempMinExp = log(tempMin);

	// Calculando as temp
	for(unsigned int ind = 0; ind < temps.size();++ind){
		double currentTemp = tempMinExp + ((tempMaxExp - tempMinExp) * ((double)ind/(double)(tempL - 1)));
		temps[ind] = exp(currentTemp);
	}
	return temps;
} 
std::deque<double> PT::tempLinear(float tempMin, float tempMax, int tempL){
	// variaveis
	std::deque<double> temps(tempL);
	
	// Calculando as temp
	for(unsigned int ind = 0; ind < temps.size();++ind){
		double currentTemp = tempMin + ((tempMax - tempMin) * ((double)ind/(double)(tempL - 1)));
		temps[ind] = currentTemp;
	}
	
	return temps;
}

std::deque<double> PT::tempILinear(float tempMin, float tempMax, int tempL){
	// variavel com as temps
	std::deque<double> temps(tempL);
	double bm = 1.0/tempMax;
	double b1 = 1.0/tempMin;
	
	// Calculando as temp
	for(unsigned int ind = 0; ind < temps.size();++ind){
		// Calcular a temp
		double bi = bm + ((b1-bm) * ((double)ind/(double)(tempL - 1)));
		temps[ind] = 1.0 / bi;
	}
	return temps;
}


void PT::stop(){
	
}

void PT::printResult(){
	
}

void PT::printBestSol(){

}

void PT::generateFiles(){

}

void PT::loadInstance(std::string filename){	
	fileName = filename;
	std::string line; 
    std::ifstream ifs;
	int i=0;
	int j=0;
	ifs.open(filename);		

	if ( ifs.is_open()){
		
		getline(ifs,line);
		std::stringstream s(line);
		getline (s,line, ' ');
		numberMachine = std::stoi(line);
		getline (s,line, ' ');		
		numberJobs = std::stoi(line);
		getline(s,line, ' ');		
		numberTools = std::stoi(line);
		
		//~ std::cout<<"Maquinas: "<<numberMachine<<"\n";
		//~ std::cout<<"tarefas: "<<numberJobs<<"\n";
		//~ std::cout<<"ferramentas: "<<numberTools<<"\n";
		
		//read machine capacity
		getline(ifs,line);
		std::stringstream cap(line);
		while ( getline(cap,line, ' ')){
			capacityMagazine.push_back(std::stoi(line));
		}
		
		//~ std::cout<<"capacidade: ";
		//~ for(auto ca:capacityMagazine)	std::cout<<ca<<", ";
		//~ std::cout<<"\n";
		
		//read tools change tax
		getline(ifs,line);
		std::stringstream tax(line);
		while ( getline(tax,line, ' ')){
			changeTax.push_back(std::stoi(line));
		}		

		//~ std::cout<<"taxa troca: ";
		//~ for(auto ta:changeTax)	std::cout<<ta<<", ";
		//~ std::cout<<"\n";
		
		
		for(int ii = 0; ii < numberMachine; ii++){
			getline(ifs,line);		
			std::stringstream ss(line);
			JobTax.push_back(std::vector<int>());
		
			while ( getline(ss,line, ' ')){
				JobTax[ii].push_back(std::stoi(line));
			}
		}

		//~ std::cout<<"job tax: ";
		//~ for(auto jo:JobTax){	
			//~ for(auto ta:jo)std::cout<<ta<<", ";
			//~ std::cout<<"\n";
		//~ }
		//~ std::cout<<"\n";
		
		
		toolJob.assign(numberTools, std::vector<bool>(numberJobs,false));
		JobTools.resize(numberJobs);
		jobInpblyMachime.resize(numberJobs);
		jobPblyMachime.resize(numberJobs);
		machimePblyJob.resize(numberMachine);
		numTools.assign(numberJobs, 0);

		// I: Ferramenta J: Jobs			
		while(getline(ifs,line)){
			
			std::stringstream sss(line);
			while ( getline (sss ,line, ' ')){
				toolJob[i][j] = std::stoi(line);
				if(toolJob[i][j]){ 
					JobTools[j].push_back(i);
					numTools[j]++;
					++sum;
					}
			j++;
			}
		j=0;
		i++;		
		}
	ifs.close();
	
	for(int jobT = 0; jobT < numTools.size(); jobT++){
		jobPblyMachime[jobT].push_back(jobT);		
		for(int capM = 0; capM < capacityMagazine.size(); capM++){
			if(capacityMagazine[capM] < numTools[jobT]){
				jobInpblyMachime[jobT].push_back(capM);
			}else{
				machimePblyJob[capM].push_back(jobT);
				jobPblyMachime[jobT].push_back(capM);
			}
		}
	}
		
		//~ std::cout<<"Numero de ferramentas por job: ";
		//~ int contador = 0;
		//~ for(auto jo:numTools){	
			//~ std::cout<<contador<<" : "<<jo<<"\n";
			//~ contador++;
		//~ }
		
		//~ std::cout<<"\n";
	
		//~ std::cout<<"Maquinas que não podem por ferramenta: ";
		//~ for(auto jo:jobInpblyMachime){	
			//~ for(auto ta:jo)std::cout<<ta<<", ";
			//~ std::cout<<"\n";
		//~ }
		//~ std::cout<<"\n";
		
		//~ std::cout<<"Tarefas que podem por maquina: ";
		//~ for(auto jo:machimePblyJob){
			//~ std::cout<<"Maquina ###################\n";	
			//~ for(auto ta:jo)std::cout<<ta<<", ";
			//~ std::cout<<"\n";
		//~ }
		//~ std::cout<<"\n";
	
	}else{
		std::cout << "Could not open! \n";
	}	
}

void PT::calcBest(){
		
	for(int i = 0; i< (numberJobs-1); ++i){
		for(int ii = i+1; ii < numberJobs; ++ii){
			int par = 0;

			for(int j = 0;j<numberTools;++j){
				if((!toolJob[j][i] && toolJob[j][ii]) || (toolJob[j][i] && !toolJob[j][ii])) ++par;
			}
			
		difJob.insert(std::make_pair(std::make_pair(i,ii),par));
		difJob.insert(std::make_pair(std::make_pair(ii,i),par));		
		}
	}
}
