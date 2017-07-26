#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <utility>
#include<time.h>
#include <math.h>
#include<sstream>
using namespace std;

//最大网络节点数量1000
#define MAX_NETWORK_NODE_NUM 1000

//最大消费节点数量500
#define MAX_CONSUME_NODE_NUM 500

//每个节点的最大链路数量
#define MAX_LINK_NUM_PER_NODE 20

//整个网络最大的边数量（在lib_io.h中已经定义，在此重复定义一下）
#define MAX_EDGE_NUM    (2000 * 20)

//链路总带宽为[0, 100]的整数
#define MIN_LINK_CAPACITY 0
#define MAX_LINK_CAPACITY 100

//单位带宽租用费为[0,100]的整数
#define MIN_UINT_CAPACITY_COST 0
#define MAX_UINT_CAPACITY_COST 100

//服务器部署成本为[0,5000]的整数
#define MIN_SERVER_COST 0
#define MAX_SERVER_COST 5000

//消费节点视频带宽需求为[0,5000]的整数
#define MIN_BANDWIDTH_DEMAND 0
#define MAX_BANDWIDTH_DEMAND 5000

//最大网络路径数量
#define MAX_PATH_NUM 50000

//单条路径节点最大数量1000
#define MAX_NODE_NUM_PER_PATH 1000

//最长程序运行时间90s
#define MAX_RUN_TIME 90

//无穷距离
#define INF 100000000

typedef  unsigned int uint;
typedef  unsigned char uchar;
typedef  pair<uint,uint> upair;//first保存最短距离，second保存顶点的编号
typedef unsigned char BYTE;

//static double round(double val)
//{
  //  return (val> 0.0) ? floor(val+ 0.5) : ceil(val- 0.5);
//}

static double random(double start, double end)
{
	//srand((unsigned)time(0));
	//return start+(end-start)*rand()/(RAND_MAX + 1.0);
	return ((double)rand()/(RAND_MAX+1.0)*(end-start) + start);
}

/*链路信息*/
typedef struct Edge{
	uint id;//边的id
	uint start_node;//开始结点
	uint end_node;//终止结点
	uint capacity;//容量
	uint per_cost;//单位容量成本
	Edge(uint i, uint s, uint e, uint c, uint p ):
	id(i),start_node(s),end_node(e),capacity(c),per_cost(p){ }

}Edge;

/*邻接表中的边信息  */
typedef struct Edge2
{
	uint end_node;
	uint capacity;
	uint per_cost;
	uint rev_id; //终点，容量（指残量网络中的），费用，反向边编号(该编号表示：顶点u，v之间存在一条边，那么顶点u的邻接表中，以v为终点的边是以v为顶点的邻接表中的第几条边，)
	Edge2(uint e,uint c,uint p,uint r) :
	end_node(e),capacity(c), per_cost(p), rev_id(r){}

}Edge2;

/*消费节点信息*/
typedef struct ConsumNode{
	uint id;//消费节点id
	uint net_node_id;//相连网络节点id
	uint bandwidth_demand;//带宽需求
	ConsumNode(uint i,uint n,uint b):
	id(i),net_node_id(n),bandwidth_demand(b){}

}ConsumNode;

/*路径信息*/
typedef struct Flow{
	vector<uint> route;//一条路径 从服务器所在网络节点到终点（消费节点所连接的网络节点）
	uint consum_id;//消费节点id
	uint bandwidth;//占用带宽大小
}Flow;


/*网络拓扑类*/
class Graph{
private:
	uint net_node_num;//网络节点数量
	uint consum_node_num;//消费节点数量
	uint link_num;//链路数量
	uint per_server_cost;//每个视频服务器成本

	/*边信息和消费节点信息*/
	vector<Edge> edge_list;
	vector<ConsumNode>  consum_node_list;
	/*容量矩阵和代价矩阵，*/
	uint **p_capacity;
	uint **p_unit_cost;
	/*各网络节点的最大输出流量*/
	vector<uint > net_node_flow;
	/*各网络节点排序后的vector<upair> 即pair<uint,uint>,first表示节点输出flow，second表示节点index*/
	vector<upair> sorted_net_node_flow;
	/*消费节点所需总流量*/
	uint total_needed_flow;

//public://?????
	/*邻接表(节点数最多为 MAX_NETWORK_NODE_NUM+2 ) 供最小流算法调用*/
	vector<Edge2 > adj_list[MAX_NETWORK_NODE_NUM+2];
	//vector<vector<Edge2>> adj_list;
public:
	/*构造一个Graph*/
	Graph();
	/*析构，释放graph所占的动态分配的内存*/
	~Graph();
	/*初始化网络topo结构*/
	void initial(char* topo[MAX_EDGE_NUM], int line_num);
	/*添加一条Edge2到adj_list中*/
	void addEdge2(uint from,uint to,uint cap,uint cost);
	/*获得单个服务器成本*/
	uint getPerServerCost();
	/*获取网络节点数*/
	uint getNetNodeNum();
	/*获取消费节点数*/
	uint getConsumNodeNum();
	/*获取链路数目*/
	uint getEdgeNum();
    uint getTotalNetNodeFlow();
	/*获取边列表*/
	vector<Edge> & getEdgeList();
	/*获取消费节点列表*/
	vector<ConsumNode> & getConsumNodeList();
	/*计算消费节点所需要的总流量*/
	void setTotalNeededFlow();
	/*获取总共所需流量*/
	uint getTotalNeededFlow();
	/*根据网络节点id得到消费节点id,若未找到则返回-1*/
	int getConsumNodeId(uint net_node_id);
	/*计算各网络节点的所允许的最大输入输出流量*/
	void setNetNodeFlow();
	/*获取指定网络节点的输出流量*/
	uint getNetNodeFlow(uint net_node_id);
	/*自定义upair<capacity,idx>排序函数*/
	static bool netNodeFlowCompare(const upair & upair1,const upair & upair2);
	/*对各网络节点的输出流量按大小进行排序 */
	void sortNetNodeFlow();
	/*获取已排序的网络节点输出量列表*/
	vector<upair>  getSortedNetNodeFlow();
	/*复制邻接表,传入参数为目的邻接表 详见MCFF类*/
	void copyAdjList(vector<Edge2 > dest_adj_list[]);
	/*返回邻接表的引用 Server类使用 用于计算消费节点间最短路径*/
	void getAdjList(vector<Edge2 > * &dest_adj_list);
	 uint getGegreeOfNetNode(uint net_node_id);//modify by hwj
};

/*多源多汇最小费用可行流 min cost feasible flow*/
class MCFF{
private:

	uint vertex_num;//网络节点+两个超级点
	uint distance[MAX_NETWORK_NODE_NUM+2];//各顶点到源点的最短距离
	uint total_uint_cost[MAX_NETWORK_NODE_NUM+2];//顶点的势，一条路径上从源点到终点的path 上的uint cost 之和
	uint pre_vertex[MAX_NETWORK_NODE_NUM+2];//最短路中的父节点
	uint pre_edg2[MAX_NETWORK_NODE_NUM+2];//最短路中的父边
	uint total_cost;//总的成本
	Graph *graph;//网络topo
	vector<Edge2 > adj_list[MAX_NETWORK_NODE_NUM+2];//所需图的邻接表的副本 每计算一次需要更新一次
	vector<Flow > best_paths;//最小成本可行流的路径列表

	vector<vector<uint> > path_list;//路径

public:
	MCFF(Graph *g);
	~MCFF();
	/*向邻接表中添加超级边*/
	void addEdge2(uint from,uint to,uint cap,uint cost);
	/*构造超级源点 超级终点对应的边添加到邻接表中 */
	void initAdjList(uint super_cource,
					 uint super_sink,
					 vector<uint> source,
					 vector<ConsumNode> consum_node_list);
	/*超级源点  超级终点  最小费用可行流 核心函数*/
	long minCostFlowCore(uint super_source,
						 uint super_sink,
						 uint need_flow,
						 vector<Flow>& best_paths);
	/*给定服务器位置 求满足需求的最小成本 及对应的路径*/
	long  getMinCostFlow(vector<uint> srever_id,vector<Flow>& best_paths );

	/*打印最小成本，路径数目，路径列表*/
	void printFlowInfo(long total_cost,vector<Flow>& flow_list);


	///*测试用 输出到屏幕*/
	//long  getMinCostFlow(vector<uint> srever_id);
	///*获取路径列表*/
	//vector<vector<int> >& getPathList();
	///*打印最小成本，路径数目，路径列表*/
	//void getPathInfo(vector<uint> srever_id);

};


class Server{
private:
    Graph *graph;
    uint min_server_num;
    uint max_server_num;
    uint max_cost;
    vector<uint> must_server_id;
    vector<uint> consum_node_net_id;
    vector<upair> sorted_net_node_flow;

public:
    Server();
    ~Server();
    void initial(Graph* g);
    uint getMinServerNum();
    uint getMaxServerNum();
    uint getMaxCost();
    vector<uint>& getMustServerID();
    vector<uint>& getConsumNodeNetID();
};



#endif
