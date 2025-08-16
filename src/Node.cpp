/*
 * Node.cpp
 * 
 * Copyright 2021 André Luis <André Luis@DESKTOP-HDL2CBS>
 *  
 * 
 */
 
#include "../include/Node.h"

Node::Node(){
}
Node::~Node(){
}

bool Node::addEdge(Node* from, Node* to){
 if (from == this){
	edgeFrom.push_back(std::make_pair(to,true));
	return true;
 } else if(to == this){
	edgeto.push_back(std::make_pair(from,false));
	return true; 
 }else{
	std::cout<<"This edge isn't related with this node!!!!"<<std::endl;
	return false;
 }
}

void Node::reset(){
	for(vector<std::pair<Node*,bool>>::iterator it = edgeto.begin(); it != edgeto.end(); it++){
		it->second = false;		
	}
}

bool Node::theEnd(){
	execAtual++;

	//if(!(*execMax <= execAtual)) std::cout<<" Exec atual:"<<execAtual<<"\n";

		
	return (*execMax <= execAtual);
}

bool Node::finish(){
	return (*execMax <= execAtual);
}

int Node::getExecAtual(){
	return execAtual;
}

void Node::printEdgeto(){
	cout<<"Node:"<<this<<endl;
	
	for(vector<std::pair<Node*,bool>>::iterator it = edgeto.begin(); it != edgeto.end(); it++){
		cout<<"POinter:"<<it->first<<" status:"<<it->second<<endl;
	}
}

void Node::printEdgeFrom(){
	cout<<"Node:"<<this<<endl;
	
	for(vector<std::pair<Node*,bool>>::iterator it = edgeFrom.begin(); it != edgeFrom.end(); it++){
		cout<<"Pointer:"<<it->first<<" status:"<<it->second<<endl;
	}
}


