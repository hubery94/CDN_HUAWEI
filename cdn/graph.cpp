#include <stdlib.h>
#include <string.h>
#include <vector>
#include "base.h"


/*构造一个Graph*/
Graph::Graph(){
	net_node_num=0;
	consum_node_num=0;
	link_num=0;
	per_server_cost=0;
	total_needed_flow=0;
}

/*析构，释放initial方法中 动态分配的内存*/
Graph::~Graph(){
	if(p_capacity){
		for(uint i=0;i<net_node_num;i++){
			delete[] p_capacity[i];
		}
		delete[] p_capacity;
	}

	if(p_unit_cost){
		for(uint i=0;i<net_node_num;i++){
			delete[] p_unit_cost[i];
		}
		delete[] p_unit_cost;
	}

}

/*初始化网络topo结构，edge_list,consum_node_list,capacity ,cost,注意三个空行的处理*/
void Graph::initial(char* topo[MAX_EDGE_NUM], int line_num){

	char *str=NULL;
	char* strcopy=NULL;
	char *word=NULL;
	uchar null_line_count=0;
	uint link_count=0;
	uint consum_node_count=0;
	vector< uint> num_list;


	for(int line=0;line<line_num;line++){
		str=topo[line];
		/*对非空行处理*/
		if(str[0]!='\r'){
			str[strlen(str)-1]='\0';
			strcopy= new char[strlen(str) + 1];
			strcpy(strcopy, str);
			word = strtok(strcopy, " ");
			num_list.push_back(atoi(word));
			while (word = strtok(NULL, " ")){
				num_list.push_back(atoi(word));
				}
			delete[] strcopy;

			/*将char* topo中的数据 转化存入到graph中*/
			/*初始化网络节点数 链路数 消费节点数,开辟p_capactity,p_unit_cost的内存*/
			if(null_line_count==0){
				net_node_num=num_list[0];
				link_num=num_list[1];
				consum_node_num=num_list[2];

				p_capacity=new uint *[net_node_num]();
				p_unit_cost=new uint *[net_node_num]();
				for(uint i=0;i<net_node_num;i++){
					p_capacity[i]=new uint[net_node_num]();
					//memset(p_capacity[i],0,sizeof(uint)*net_node_num);
					p_unit_cost[i]=new uint[net_node_num]();
					//memset(p_unit_cost[i],0,sizeof(uint)*net_node_num);
				}
			}
			/*初始化单个服务器成本*/
			if(null_line_count==1){
				per_server_cost=num_list[0];
			}

			/*初始化 边 即edge_list*/
			if(null_line_count==2){
				edge_list.push_back(Edge(link_count,num_list[0],num_list[1],num_list[2],num_list[3]));
				addEdge2(num_list[0],num_list[1],num_list[2],num_list[3]);

				/*为每条边（一对定点）设置对应的容量和代价*/
				p_capacity[num_list[0]][num_list[1]]=num_list[2];
				p_capacity[num_list[1]][num_list[0]]=num_list[2];

				p_unit_cost[num_list[0]][num_list[1]]=num_list[3];
				p_unit_cost[num_list[1]][num_list[0]]=num_list[3];
				link_count++;
			}
			/*初始化消费节点 即consum_node_list*/
			if(null_line_count==3){
				consum_node_list.push_back(ConsumNode(num_list[0],num_list[1],num_list[2]));
				consum_node_count++;
			}

			num_list.clear();

		}else{
			null_line_count++;
		}
	}

	setTotalNeededFlow();
	setNetNodeFlow();
	sortNetNodeFlow();
}


/*获取网络节点数*/
uint Graph::getNetNodeNum(){
	return net_node_num;
}
/*获取消费节点数目*/
uint Graph::getConsumNodeNum(){
	return consum_node_num;
}

/*获取边的数目*/
uint Graph::getEdgeNum(){
	return link_num;
}
vector<Edge> & Graph::getEdgeList(){
	return edge_list;
}

/*获得单个服务器成本*/
uint Graph::getPerServerCost(){
	return per_server_cost;
}
/*获取消费节点列表*/
vector<ConsumNode> & Graph::getConsumNodeList(){
	return consum_node_list;
}

/*计算消费节点所需要的总流量*/
void Graph::setTotalNeededFlow(){
	total_needed_flow=0; //add by hwj
	for(int i=0;i<consum_node_list.size();i++){
		total_needed_flow+=consum_node_list[i].bandwidth_demand;
	}
}

/*获取总共所需流量*/
uint Graph::getTotalNeededFlow(){
	return total_needed_flow;
}

void Graph::addEdge2(uint from,uint to,uint cap,uint cost){
	adj_list[from].push_back(Edge2(to,cap,cost,adj_list[to].size()));
	adj_list[to].push_back(Edge2(from,cap,cost,adj_list[from].size()-1));
}

int Graph::getConsumNodeId(uint net_node_id){
	uint k=-1;
	for(uint i=0;i<consum_node_list.size();i++){
		if(consum_node_list[i].net_node_id==net_node_id){
			k=i;
			break;
		}
	}
	if(k==-1)
		return consum_node_list[k].id;
//        return -1;
        else
		return k;
}

/*计算各网络节点的所允许的最大输入输出流量*/
void Graph::setNetNodeFlow(){
	uint flow_sum=0;
	for(uint i=0;i<net_node_num;i++){
		for(uint j=0;j<net_node_num;j++){
			flow_sum+=p_capacity[i][j];
		}
		net_node_flow.push_back(flow_sum);
		flow_sum=0;
	}
}

//Added by fqyuan 3.30
uint Graph:: getTotalNetNodeFlow(){
    uint totalNetNodeFlow = 0;
    for(int i=0; i<net_node_num; i++)
        totalNetNodeFlow += getNetNodeFlow(i);
    return totalNetNodeFlow;
}
/*获取指定网络节点的输出流量*/
uint Graph::getNetNodeFlow(uint net_node_id){
	return net_node_flow[net_node_id];
}

void Graph::copyAdjList(vector<Edge2 > dest_adj_list[]){

	copy(adj_list,adj_list+MAX_NETWORK_NODE_NUM+2,dest_adj_list);
}

/*返回邻接表的引用 Server类使用 用于计算消费节点间最短路径*/
void Graph::getAdjList(vector<Edge2 >* &dest_adj_list){
	//dest_adj_list =(vector<Edge2 > **)&((vector<Edge2 > *)adj_list) ;
	//*dest_adj_list
	//vector<Edge2 > * tmp=adj_list;
	dest_adj_list=adj_list;

}

/*自定义upair<capacity,idx>排序函数*/
bool Graph::netNodeFlowCompare(const upair &upair1,const upair &upair2){
	return upair1.first>upair2.first;
}
/*对各网络节点的输出流量按大小进行排序 */
void Graph::sortNetNodeFlow(){
	sorted_net_node_flow.clear();// add by hwj, clear before assignment
	for(int i=0;i<net_node_flow.size();i++){
		upair u(net_node_flow[i],i);
		sorted_net_node_flow.push_back(u);
	}

	sort(sorted_net_node_flow.begin(),
		 sorted_net_node_flow.end(),
		 netNodeFlowCompare);
}

/*获取已排序的网络节点输出量列表*/
vector<upair>  Graph::getSortedNetNodeFlow(){
	return sorted_net_node_flow;
}


//0331 nodify by hwj
// calaulate the weighted degree of each net node
uint Graph::getGegreeOfNetNode(uint net_node_id){
uint degree=0;
for(int j=0;j<net_node_num;j++){

   degree+=p_unit_cost[net_node_id][j];

}
 return degree;

}
