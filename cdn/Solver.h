/**
* Copyright (C) 2017 All right reserved.

* @ File:		Population.h
* @ VERSION:	1.0
* @ Author:		HWJ(2011huang@whu.edu.cn)
* @ Brief:		Population Definition of GA;

* Detail:

* TODO
* History:	    2017/3/20 created by HWJ
*
* Note:
*/

#ifndef __SOLVER_H__
#define __SOLVER_H__
#pragma once
#include "GA.h"
class Solver
{
public:
	Solver(Graph* g);
	~Solver(void);
	bool initial(const int iter_times,const int population_scale, int repeat_times, int tournament_size, double mutationRate);
	bool GASolver();
	bool GASolverNew();
    bool GASolverEasy();

	//wjs 3.31
    Population  getInitPop(Population* pre_pop);
    void setMustLocationID(Population* pop,vector<uint> & must_server_id);

	string getContent();
private:
    //get the variable population scale
    int get_variable_population_scale(int generation, int average_population_size, int amplitude, int variation);
	int MAX_ITER_TIMES;     // max iter times
	int POPULATION_SCALE;   // population scale
	int MAX_REPEAT_TIMES;   // max repeat times
    int TOURNAMENT_SIZE;    //used in selection in GA, added 3.27
    double MUTATION_PROBABILITY;
	Graph* graph;
	vector<Flow> best_path;
	long min_cost;
	vector<uint> best_server;
};
#endif // __SOLVER_H__
