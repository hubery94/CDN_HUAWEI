#include "base.h"

Server::Server(){
    
    
}
Server::~Server(){
}



void Server::initial(Graph* g){
    if(g){
        graph=g;
        //net_node_num=graph->getNetNodeNum();
        max_server_num=graph->getConsumNodeNum();
        
        
        max_cost=max_server_num*graph->getPerServerCost();
        
        //selected_server_id.clear();
        
        must_server_id.clear();
        vector<ConsumNode > consum_node_list=graph->getConsumNodeList();
        consum_node_net_id.clear();
        for(uint i=0;i<consum_node_list.size();i++){
            if(consum_node_list[i].bandwidth_demand>graph->getNetNodeFlow(consum_node_list[i].net_node_id) ){
                must_server_id.push_back(consum_node_list[i].net_node_id);
            }
            //set consum_node_net_id
            consum_node_net_id.push_back(consum_node_list[i].net_node_id);
            
        }
        
        sorted_net_node_flow=graph->getSortedNetNodeFlow();
        uint sum_flow=0;
        for(uint i=0;i<sorted_net_node_flow.size();i++){
            sum_flow+=sorted_net_node_flow[i].first;
            if(sum_flow>=graph->getTotalNeededFlow()){
                min_server_num=i+1;
                break;
            }
        }
        
        
    }else{
        cout<<"Server::initial(Graph* graph)failed"<<endl;
    }
}

uint Server::getMinServerNum(){
    return min_server_num;
}
uint Server::getMaxServerNum(){
    return max_server_num;
}
uint Server::getMaxCost(){
    return max_cost;
}

vector<uint>& Server::getConsumNodeNetID(){
    return consum_node_net_id;
}

vector<uint>& Server::getMustServerID(){
    return must_server_id;
}

