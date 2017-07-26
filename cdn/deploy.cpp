#include "deploy.h"
#include <stdio.h>

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    
    int iter_times=500;       //max iter times
    
    Graph* graph=new Graph();
    graph->initial(topo,line_num);
    
    int node_num=graph->getNetNodeNum();
    Solver* gaSolver=new Solver(graph);
    
    if(node_num<=250)
    {
        int repeat_times=30;      //max repeat times of cost
        int population_scale=500; //500
        int tournament_size = 5;  //5
        double mutateRate =0.0056; // 1.0/node_num; 
        
        gaSolver->initial(iter_times,population_scale,repeat_times,tournament_size,mutateRate);
        gaSolver->GASolverEasy();
    }
    else if(node_num>250 && node_num<600){
        int repeat_times=30;      //max repeat times of cost
        int population_scale=400; //500
        int tournament_size =5;  //5
        double mutateRate = 0.0068;  // 1.0/node_num;0.0068
        
        gaSolver->initial(iter_times,population_scale,repeat_times,tournament_size,mutateRate);
        gaSolver->GASolverEasy();
    }
    else
    {
        int repeat_times=10;      //max repeat times of cost
        int population_scale=100; //population scale of each generation
        int tournament_size = 10;
        double mutateRate =0.7/node_num;    //
        
        gaSolver->initial(iter_times,population_scale,repeat_times,tournament_size,mutateRate);
        gaSolver->GASolverNew();
    }
    
    
    string content=gaSolver->getContent();
    write_result(content.c_str(), filename);
    //------modfiy by wjs 0329----------
    delete gaSolver;
    delete graph;
    
}
