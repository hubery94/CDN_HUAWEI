#include "Solver.h"
#define PRINT_ITERATION   //NEED COMMENT BEFORE SUBMIT
#define MAX_ALLOWED_TIME 85// NEED MODIFY BEFORE SUBMIT

Solver::Solver(Graph* g)
{
    graph=g;
}


Solver::~Solver(void)
{
}

/****************************************************
*
* Function:	initial
*
* Brief:	Init  set max iter times, population scale and max repeat times
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/

bool Solver::initial(const int iter_times,const int population_scale, int repeat_times, int tournament_size,double mutationRate)
{
    if(iter_times<1||population_scale<1)
        return false;
    MAX_ITER_TIMES=iter_times;
    POPULATION_SCALE=population_scale;
    MAX_REPEAT_TIMES=repeat_times;
    TOURNAMENT_SIZE = tournament_size;
    MUTATION_PROBABILITY = mutationRate;
    return true;
}


/****************************************************
*
* Function:	get_variable_population_scale
*
* Brief:	get variable population scale, n(t)=int{average_population_size+amplitude-(2*amplitude)/(variation-1){}}
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
****************************************************/
int Solver::get_variable_population_scale(int generation, int average_population_size, int amplitude, int variation)
{

    int t_size=0;
    int num1=generation-variation*(int)((generation-1)/variation)-1;
    int num2=2*amplitude/(variation-1)*num1;
    t_size=(int)(average_population_size+amplitude-num2);

    return t_size;

}




/****************************************************
*
* Function:	GASolver
*
* Brief:	Main module of Genetic Algorithm
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
bool Solver::GASolver()
{
    clock_t start,finish;
    start=clock();
    MCFF3* mc = new MCFF3(graph);
    GA* genertic=new GA(TOURNAMENT_SIZE,MUTATION_PROBABILITY);
    int netSize = graph->getNetNodeNum();
    Location bestLocation;
    Server* server=new Server();
    server->initial(graph);
    vector<uint> comsum_node_net_id=server->getConsumNodeNetID();
    Population* initialPop = new Population(POPULATION_SCALE, netSize, true);
  
    double fittness=0;
    bestLocation=initialPop->getFittest(mc,fittness);
    min_cost=(long)(1/fittness);
    best_path.clear();
    best_path=initialPop->getBestPath();
    Population myPop=*initialPop;
    int count=0;
    double cost=0;
    for(int i=0; i<MAX_ITER_TIMES; i++)
    {
        finish=clock();
        if((double)(((finish-start)/CLOCKS_PER_SEC))>MAX_ALLOWED_TIME)
        {
            break;
        }
        if(count>MAX_REPEAT_TIMES)
        {
            break;
        }

        myPop=genertic->evolvePopulation(&myPop, netSize, mc, bestLocation,POPULATION_SCALE,i);


        bestLocation=myPop.getFittest(mc,fittness);
        long cadidate_cost=(long)(1/fittness);
        if(cadidate_cost==cost&&i>0)
        {
            count++;
        }
        else
        {
            count=0;
        }
        if(cadidate_cost<min_cost)
        {
            best_path.clear();
            min_cost=cadidate_cost;
            best_path=myPop.getBestPath();
        }
        cost=min_cost;
#ifdef PRINT_ITERATION
        printf("Iteration: %d :%ld \n",i, min_cost);
#endif
    }
    delete mc;
    delete genertic;
    delete server;
    delete initialPop;
    return true;
}


string Int_to_String(int n)
{
    ostringstream stream;
    stream<<n;
    return stream.str();
}


/****************************************************
*
* Function:	getContent
*
* Brief:	Write the result path into string
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
string Solver::getContent()
{

    if(best_path.size()==0)
    {
        const char * out_put="NA";
        return out_put;
    }
    string content="";
    int route_num=best_path.size();
    //write the total flow num
    content.append(Int_to_String(route_num));
    //write a blank row
    content.append("\n");
    vector<uint> tmp_route;
    for(int i=0; i<route_num; i++)
    {
        // write each flow
        content.append("\n");
        Flow tmp_flow=best_path[i];
        tmp_route.clear();
        tmp_route=tmp_flow.route;
        //write route first
        for(int j=0; j<tmp_route.size(); j++)
        {
            content.append(Int_to_String(tmp_route[j]));
            content.append(" ");
        }
        //write the consumer ID
        content.append(Int_to_String(tmp_flow.consum_id));
        content.append(" ");
        //write the bandwidth
        content.append(Int_to_String(tmp_flow.bandwidth));
    }
    return content;
}



Population  Solver::getInitPop(Population* pre_pop){

	Population * init_pop=NULL;
	vector<uint> feasible_loc_idx;
	double * p_all_fitness=pre_pop->getAllFitness();
	for(int i=0;i<pre_pop->size();i++){
		if(p_all_fitness[i] > (1.0/INF)){
			feasible_loc_idx.push_back(i);
		}
	}

	int pop_size=feasible_loc_idx.size();
	init_pop=new Population(pop_size,graph->getNetNodeNum(),false);

	for(int i=0;i<pop_size;i++){
		init_pop->saveLocation(i,pre_pop->getLocation(feasible_loc_idx[i]));
		init_pop->setFitness(i,p_all_fitness[feasible_loc_idx[i]]);
	}

	return *init_pop;

}


void Solver::setMustLocationID(Population* pop,vector<uint> & must_server_id){
	Location * p_locations=pop->getLocations();
	Location loc;
	BYTE * p_genes;
	int pop_size=pop->size();

	int i, j;

	for(i=0;i<pop_size;i++){
		loc=p_locations[i];
		p_genes=loc.getGenesPtr();
		for(j=0;j<must_server_id.size();j++){
			p_genes[must_server_id[j] ]=1;
		}
	}
}
