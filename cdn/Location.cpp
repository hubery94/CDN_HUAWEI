#include "Location.h"

Location::Location(void)
{


}

/****************************************************
 *
 * Function:	Location::Location
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
Location::Location(int node_num)
{
    genes=new BYTE[node_num];
    net_node_num=node_num;
}

Location::Location(int node_num, Graph *g)
{
    genes=new BYTE[node_num];
    net_node_num=node_num;
    graph = g;
}
Location::~Location(void)
{

}

/****************************************************
*
* Function:	generateLocation
*
* Brief:	Generate individuals with respect to flow capacity
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
void Location::generateLocation(int count,Graph* g){
   int flow_capacity=0;
   int weighted_degree=0;
   double p=0;
   if(net_node_num!=count)
    {
        return;
    }
    vector<upair> sort_flow=g->getSortedNetNodeFlow();
    upair best=sort_flow[0];
    int flow_max=best.first;
    int node_id=best.second;
    upair worst=sort_flow[sort_flow.size()-1];
    int flow_min=best.first;
    int worst_id=best.second;
    int min_norm=flow_min;
    int normlization=flow_max;

    for (int i = 0; i < count; i++)
    {
        flow_capacity=g->getNetNodeFlow(i);
        weighted_degree=g->getGegreeOfNetNode(i);
        p=(double)(flow_capacity)/normlization;
        p=min(1.0,max(0.2,p));
        if(random(0,1)<p )
        {
            genes[i] = (BYTE)1;
        }
        else
        {
            genes[i] = (BYTE)0;
        }


    }
}

/****************************************************
*
* Function:	generateLocation
*
* Brief:	Generate individuals with respect to consumer node
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
void Location::generateLocationByConsumer(int count, Location* consumer_loc)
{
    if(count!=this->net_node_num)
        return;
    int yes=0;
    int no=0;
    countServer(*consumer_loc,yes,no);
    int yes_num=0;
    int no_num=(int)random(0,no);
    bool add=false;
    bool del=false;
    bool swap=false;
    for(int i=0; i<consumer_loc->size(); i++)
    {
        this->setGene(i,consumer_loc->getGene(i));
    }
    int c=0;
    if(random(0,1)>0.1)
    {
        yes_num=(int)random(0,yes);
        for(int i=0; i<consumer_loc->size(); i++)
        {
            if(consumer_loc->getGene(i)==1)
            {
                if(c==yes_num)
                {
                    this->setGene(i,(BYTE)0);
                    del=true;
                }

                c++;
            }

        }
    }
    c=0;
    if(random(0,1)>0.9&&del)
    {
        no_num=(int)random(0,no);
        for(int i=0; i<consumer_loc->size(); i++)
        {
            if(consumer_loc->getGene(i)==0)
            {
                if(c==no_num)
                {
                    this->setGene(i,(BYTE)1);
                    add=true;
                }

                c++;
            }

        }
    }
    if(random(0,1)>0.05)
    {
        int poss_node=(int)random(0,yes);
        int empty_node=(int)random(0,no);
        int poss_count=0;
        int empty_count=0;
        for(int i=0; i<consumer_loc->size(); i++)
        {
            if((int)consumer_loc->getGene(i)==0)
            {
                if(empty_count==empty_node)
                {
                    this->setGene(i,(BYTE)1);
                }

                empty_count++;
            }
            else
            {

                if(poss_count==poss_node)
                {
                    this->setGene(i,(BYTE)0);

                }

                poss_count++;
            }

        }
    }
}
void Location::countServer(Location& loc,int &yes, int & no)
{

    yes=0;
    no=0;
    for(int i=0; i<loc.size(); i++)
    {
        if((int)loc.getGene(i)==0)
        {
            yes++;
        }
        else
        {
            no++;

        }

    }


}
/****************************************************
*
* Function:	generateLocation
*
* Brief:	Generate individuals random
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
void Location::generateLocation(int count)
{
    if(net_node_num!=count)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
       
        if(random(0,1)>0.90)
        {
            genes[i] = (BYTE)1;
        }
        else
        {
            genes[i] = (BYTE)0;
        }
    }

}


/****************************************************
*
* Function:	generateLocation
*
* Brief:	Generate consumer location
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
void Location::generateConsumLocation(int count,vector<uint> & consum_node_net_id)
{
    if(net_node_num!=count)
    {
        return;
    }
    int i;
    //reset
    for( i=0; i<count; i++)
    {
        genes[i] = 0;
    }

    for(i=0; i<consum_node_net_id.size(); i++)
    {
        uint net_node_id=consum_node_net_id[i];
        genes[net_node_id]=(BYTE) 1;
    }

}



/****************************************************
*
* Function:	getFitness
*
* Brief:	Compute fitness of each individual
* Input:
* Output:
* Return:
* Others:
*
* History:	2017/3/20 created by hwj
*
****************************************************/
double Location::getFitness(MCFF3* m)
{
    vector<uint> tmp_servers;
    vector<Flow> tmp_paths;
    tmp_servers.clear();
    tmp_paths.clear();
    for (int i = 0; i <size(); i++)
    {
        if (genes[i] == 1)
        {
            tmp_servers.push_back(i);
        }
    }
    int curVal = 0;
    if(tmp_servers.empty())
        curVal = -1;
    else
        curVal =m->getMinCost(tmp_servers);
    if (curVal == -1)
    {
        curVal = INF;
    }
    fitness = 1.0 / curVal;
    return fitness;

}

int Location::size() const
{
    return net_node_num;
}

void Location::setGene(int index, BYTE value)
{
    genes[index] = value;
    fitness = 0;

}

BYTE Location::getGene(int index)
{
    return genes[index];
}

BYTE* Location::getGenesPtr(){
    return genes;
}
