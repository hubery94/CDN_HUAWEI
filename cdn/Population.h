/**
* Copyright (C) 2017 All right reserved.

* @ File:		Population.h
* @ VERSION:	1.0
* @ Author:		HWJ(2011huang@whu.edu.cn)
* @ Brief:		Population Definition of GA;

* Detail:

* TODO
* History:		2017/3/20 created by HWJ
*
* Note:
*/

#ifndef __POPULATION_H__
#define __POPULATION_H__
#pragma once
#include "Location.h"

class Population
{
public:
	Population(int populationSize, int netSize, bool initialise);
	//added by wjs 0403
	Population(int populationSize, int netSize, bool initialise,int case_order);
	Population(int populationSize, int netSize, bool initialise,Graph*g);
    Population(const Population & pop);
    //Added by fqyuan 3.30
    Population(int populationSize, int netSize, bool initialise, int minGene, int maxGene, Graph * graph);
    Population(int popuLationSize, int netSize, bool initialise, int minGene, int maxGene);
	~Population(void);
	Location getFittest(MCFF3* mc, double &fittness);
	void saveLocation(int index, Location location);
	Location getLocation(int index);
	int size();
	vector<Flow> getBestPath();

//added by wjs---------0401--------------------
	vector<Flow> getBestPath(MCFF3* mc);

	//long getMinCost();
	vector<uint> getBestServer();
	double * all_fitness;
	//bool checkLoc(Location loc);
    Location *getLocations();
    Location getRandomAvailabeLocation();
    double* getAllFitness();

  //added by wjs---------0401--------------------
	void setFitness(int index,double fitness);
private:
	Location* locations;
	int population_size;
	vector<Flow> best_path;
	long min_cost;
	vector<uint> best_server;
    vector<uint> location2Vector(Location* loc);
    //Server* server;
};

#endif  //__POPULATION_H__
