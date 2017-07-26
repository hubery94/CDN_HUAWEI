#include "GA.h"
#define _REINIIION
/****************************************************
 *
 * Function:	GA::GA
 *
 * Brief:	Constructor set mutation rate
 * Input:
 * Output:
 * Return:
 * Others:
 *
 * History:	2017/3/20 created by hwj
 *
 ****************************************************/

GA::GA(int tourSize, double mutRate){
    uniformRate= 0.5;
    mutationRate=mutRate;
    iter=0;
    tournamentSize = tourSize;
}


GA::~GA(void){
    
}
/****************************************************
 *
 * Function:	evolvePopulation
 *
 * Brief:	Evolve a populatin
 * Input:
 * Output:
 * Return:
 * Others:
 *
 * History:	2017/3/20 created by hwj
 *
 ****************************************************/
Population GA::evolvePopulation(Population* pop, int netSize, MCFF3* mc,Location bestLocation,int pop_scale_now, int iter)
{
    
    Population* newPopulation = new Population(pop_scale_now, netSize, false);
    // Keep our best individual
    int elitismOffset = 0;
    if (elitism){
        newPopulation->saveLocation(0,bestLocation);
        elitismOffset = 1;
    }
    
    /*
     * CrossOver population: Loop over popution's size and create
     * individuals from current population
     */
    
    srand((int)time(0));
    //for (int i = elitismOffset; i < newPopulation->size(); i++) {
    for (int i = elitismOffset; i < pop_scale_now; i++)
    {
        // Select parents:
        Location parent1 = tournamentSelection(pop, netSize, mc,iter);
        Location parent2 = tournamentSelection(pop, netSize, mc,iter);
        // Crossover parents
        Location child = crossover(parent1, parent2, netSize);
        // Add child to the new population
        newPopulation->saveLocation(i, child);
    }
    
    // Mutate population
    // for (int i = elitismOffset; i < newPopulation->size(); i++) {
    for (int i = elitismOffset; i <pop_scale_now; i++)
    {
        mutate(newPopulation->getLocation(i), iter);
    }
    
    return *newPopulation;
    
}
Population GA::evolvePopulationEasy(Population* pop, int netSize, MCFF3* mc,Location bestLocation,int pop_scale_now, int iter)
{
    
    Population* newPopulation = new Population(pop_scale_now, netSize, false);
    // Keep our best individual
    int elitismOffset = 0;
    if (elitism){
        newPopulation->saveLocation(0,bestLocation);
        elitismOffset = 1;
    }
    
    /*
     * CrossOver population: Loop over popution's size and create
     * individuals from current population
     */
    
    srand((int)time(0));
    //for (int i = elitismOffset; i < newPopulation->size(); i++) {
    for (int i = elitismOffset; i < pop_scale_now; i++)
    {
        // Select parents:
        Location parent1 = tournamentSelectionEasy(pop, netSize, mc,iter);
        Location parent2 = tournamentSelectionEasy(pop, netSize, mc,iter);
        // Crossover parents
        Location child = crossoverEasy(parent1, parent2, netSize);
        // Add child to the new population
        newPopulation->saveLocation(i, child);
    }
    
    // Mutate population
    // for (int i = elitismOffset; i < newPopulation->size(); i++) {
    for (int i = elitismOffset; i <pop_scale_now; i++)
    {
        mutateEasy(newPopulation->getLocation(i));
    }
    
    return *newPopulation;
    
}

/****************************************************
 *
 * Function:	crossover
 *
 * Brief:	Applies crossover to a set of parents
 *           and creates offspring
 * Input:
 * Output:
 * Return:
 * Others:
 *
 * History:	2017/3/20 created by hwj
 *
 ****************************************************/

Location GA::crossover(Location parent1, Location parent2, int netSize)
{
    
    Location* child = new Location(netSize);
    // Loop through genes:
    
    for (int i = 0; i < parent1.size(); i++)
    {
        // Crossover
        if (random(0,1) <= uniformRate)
        {
            child->setGene(i, parent1.getGene(i));
        }
        else
            child->setGene(i, parent2.getGene(i));
    }
    return *child;
    
    
}

Location GA::crossoverEasy(Location parent1, Location parent2, int netSize)
{
    
    Location* child = new Location(netSize);
    // Loop through genes:
    
    for (int i = 0; i < parent1.size(); i++)
    {
        // Crossover
        if (random(0,1) <= uniformRate)
        {
            child->setGene(i, parent1.getGene(i));
        }
        else
            child->setGene(i, parent2.getGene(i));
    }
    return *child;
    
    
}

/****************************************************
 *
 * Function:	mutate
 *
 * Brief:	Mutate an location
 * Input:
 * Output:
 * Return:
 * Others:
 *
 * History:	2017/3/20 created by hwj
 *
 ****************************************************/


void GA::mutate(Location loc, int iter)
{
    // Loop through genes:
    double c1 = 0.0001;
    double curMutationRate = (mutationRate+iter*c1)>=mutationRate*5?mutationRate*5:(mutationRate+iter*c1);
    for (int i = 0; i < loc.size(); i++)
    {
        if (random(0,1) <= curMutationRate)
        {
            // Create random gene
            BYTE gene = (BYTE)round(random(0,1));
            loc.setGene(i, gene);
        }
    }
}

void GA::mutateEasy(Location loc)
{
    
    
    // Loop through genes:
    for (int i = 0; i < loc.size(); i++)
    {
        if (random(0,1) <= mutationRate)
        {
            // Create random gene
            BYTE gene = (BYTE)round(random(0,1));
            loc.setGene(i, gene);
        }
    }
}

/****************************************************
 *
 * Function:	tournamentSelection
 *
 * Brief:	Select candidates location for crossover
 * Input:
 * Output:
 * Return:
 * Others:
 *
 * History:	2017/3/20 created by hwj
 *
 ****************************************************/
Location GA::tournamentSelection(Population* pop, int netSize, MCFF3* mc, int iter)
{
    
    //srand((int)time(0));
    Location bestLocation;
    // Create a tournament population
    // int c_tournamentSize=std::max(2,tournamentSize-(int)(iter/20));
    // For each place in the tournament get a random candidate location and add it
    double fittness=0;
    int lastId= 0;
    bool em=true;
    for (int i = 0; i < tournamentSize;i++)
    {
        
        int randomId = (int) (random(0,1) * pop->size());
        if((int)(1/pop->all_fitness[randomId])==INF || randomId==lastId)
        {
            continue;
        }
        
        if(pop->all_fitness[randomId]>fittness)
        {
            bestLocation=pop->getLocation(randomId);
            fittness=pop->all_fitness[randomId];
            em=false;
        }
        //        i++;
        lastId = randomId;
    }
    
    if(em){
        bestLocation=pop->getLocation(0);
        fittness=pop->all_fitness[0];
    }
    //-- return bestLocation;
    return  Location(bestLocation);
    
}

Location GA::tournamentSelectionEasy(Population* pop, int netSize, MCFF3* mc, int iter)
{
    
    //srand((int)time(0));
    Location bestLocation;
    // Create a tournament population
    // int c_tournamentSize=std::max(2,tournamentSize-(int)(iter/20));
    // For each place in the tournament get a random candidate location and add it
    double fittness=0;
    int lastId= 0;
    bool em=true;
    for (int i = 0; i < tournamentSize;i++)
    {
        
        int randomId = (int) (random(0,1) * pop->size());
        if((int)(1/pop->all_fitness[randomId])==INF || randomId==lastId)
        {
            continue;
        }
        
        if(pop->all_fitness[randomId]>fittness)
        {
            bestLocation=pop->getLocation(randomId);
            fittness=pop->all_fitness[randomId];
            em=false;
        }
        
        lastId = randomId;
        //        i++;
    }
    
    if(em){
        bestLocation=pop->getLocation(0);
        fittness=pop->all_fitness[0];
        
    }
    //-- return bestLocation;
    return  Location(bestLocation);
    
}
