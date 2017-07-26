#include "Population.h"

/****************************************************
*
* Function:	Population::Population
*
* Brief:	Constructor
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
Population::Population(int populationSize, int netSize, bool initialise)
{
    locations=new Location[populationSize];
    all_fitness=new double[populationSize];
    population_size=populationSize;
    srand((int)time(0));
    if (initialise)
    {
        // Loop and create new individuals
        // printf("\n Initial Population:-----\n");
        int i = 0;
        while (i <population_size)
        {
            Location* newLocation = new Location(netSize);
            newLocation->generateLocation(netSize);
            saveLocation(i, *newLocation);
            i++;
        }

    }

}


Population::~Population(void){
    
}

/****************************************************
*
* Function:	Population::Population
*
* Brief:	Copy Constructor
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
Population::Population(const Population & pop){
    population_size=pop.population_size;
    locations=new Location[population_size];
    all_fitness=new double[population_size];
    copy(pop.locations,pop.locations+population_size,locations);
    copy(pop.all_fitness,pop.all_fitness+population_size,all_fitness);
    min_cost=pop.min_cost;
    best_path=pop.best_path;
    best_server=pop.best_server;
    
}



/****************************************************
*
* Function:	location2Vector
*
* Brief:	Convert location information to vector
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
vector<uint> Population::location2Vector(Location* loc)
{
    vector<uint> tmp_servers;
    for (int i = 0; i <loc->size(); i++)
    {
        if (loc->getGene(i) == 1)
        {
            tmp_servers.push_back(i);
        }
    }
    return tmp_servers;
}

/****************************************************
*
* Function:	getFittest
*
* Brief:	Get the best individual
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
Location Population::getFittest(MCFF3* mc,double &fittness)
{

    Location bestLoc = locations[0];
    fittness=bestLoc.getFitness(mc);
    all_fitness[0]=fittness;
    double tmp=0;
    for (int i = 1; i < population_size; i++)
    {
        tmp=locations[i].getFitness(mc);
        all_fitness[i]=tmp;
        if (fittness <= tmp)
        {
            bestLoc = locations[i];
            fittness=tmp;
        }
    }

    best_server=location2Vector(&bestLoc);

    return bestLoc;
}


vector<Flow> Population::getBestPath()
{

    return best_path;

}

vector<Flow> Population::getBestPath(MCFF3* mc)
{
	long est=mc->getMinCostFlow(best_server,best_path);
	return best_path;
}


vector<uint> Population::getBestServer()
{
    return  best_server;
}

void Population::saveLocation(int index, Location location)
{

    locations[index] = location;

}

Location Population::getLocation(int index)
{
    return locations[index];
}

int Population::size()
{

    return population_size;
}
Location * Population::getLocations(){
    return locations;
}
double* Population::getAllFitness(){
    return all_fitness;
}

void Population::setFitness(int index,double fitness){
	all_fitness[index]=fitness;
}