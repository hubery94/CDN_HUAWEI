/**
 * Copyright (C) 2017 All right reserved.
 
 * @ File:		GA.h
 * @ VERSION:	1.0
 * @ Author:		HWJ(2011huang@whu.edu.cn)
 * @ Brief:		Genetic Algorithm;
 
 * Detail:
 
 * TODO
 * History:		2017/3/20 created by HWJ
 *
 * Note:
 */

#ifndef __GENETIC_H__
#define __GENETIC_H__
#pragma once
#include "Location.h"
#include "Population.h"

class GA{
    
public:
    GA(int tournamentSize, double mutateRate);
    ~GA(void);
    Population evolvePopulation(Population* pop, int netSize, MCFF3* mc, Location bestLocation,int pop_scale_now,int iter);
    Population evolvePopulationEasy(Population* pop, int netSize, MCFF3* mc, Location bestLocation,int pop_scale_now,int iter);
    
private:
    Location crossover(Location parent1, Location parent2, int netSize);
    Location crossoverEasy(Location parent1, Location parent2, int netSize);
    void mutateEasy(Location loc);
    void mutate(Location loc, int iter);
    Location tournamentSelection(Population* pop, int netSize, MCFF3* mc,int iter);
    Location tournamentSelectionEasy(Population* pop, int netSize, MCFF3* mc,int iter);
    Location bestLocation;
    double uniformRate;   //uniformRate for crossover
    double mutationRate;  // mutation rate
    int tournamentSize; //selection range, initial value is 5,
    static const bool elitism=true;      // use elitism or not
    int iter;
};

#endif //__GENETIC_H__
