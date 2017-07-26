/**
* Copyright (C) 2017 All right reserved.

* @ File:		Location.h
* @ VERSION:	1.0
* @ Author:		HWJ(2011huang@whu.edu.cn)
* @ Brief:		Facility Location;

* Detail:

* TODO
* History:		2017/3/20 created by HWJ
*
* Note:
*/

#ifndef __LOCATION_H__
#define __LOCATION_H__
#pragma once
#include "base.h"
#include "mcff3.h"
class Location
{
public:
	Location(void);
	Location(int net_node_num);
    Location(int net_node_num, Graph* graph);
	~Location(void);
	void generateLocation(int count);

//added by wjs 0403
	void generateLocation(int count,int case_order);

	void generateLocation1(int count, int minCnt, int maxCnt);
    void generateConsumLocation(int count,vector<uint> & consum_node_net_id);
    void generateLocationGuess(int count);
    void generateLocationByProbability(int count, int minCnt, int maxCnt);
	double getFitness(MCFF3* m);
	void generateLocationByConsumer(int count,Location* consumer_loc);
	void generateLocation(int count,Graph* g);
	int size() const;
	void setGene(int index, BYTE value);
	BYTE getGene(int index);
    BYTE* getGenesPtr();
private:
	int net_node_num;
	BYTE* genes;
	double fitness;
	void countServer(Location& loc,int &yes, int & no);
    Graph *graph;
};

#endif  //__LOCATION_H__
