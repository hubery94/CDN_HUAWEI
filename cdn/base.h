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

//�������ڵ�����1000
#define MAX_NETWORK_NODE_NUM 1000

//������ѽڵ�����500
#define MAX_CONSUME_NODE_NUM 500

//ÿ���ڵ�������·����
#define MAX_LINK_NUM_PER_NODE 20

//�����������ı���������lib_io.h���Ѿ����壬�ڴ��ظ�����һ�£�
#define MAX_EDGE_NUM    (2000 * 20)

//��·�ܴ���Ϊ[0, 100]������
#define MIN_LINK_CAPACITY 0
#define MAX_LINK_CAPACITY 100

//��λ�������÷�Ϊ[0,100]������
#define MIN_UINT_CAPACITY_COST 0
#define MAX_UINT_CAPACITY_COST 100

//����������ɱ�Ϊ[0,5000]������
#define MIN_SERVER_COST 0
#define MAX_SERVER_COST 5000

//���ѽڵ���Ƶ��������Ϊ[0,5000]������
#define MIN_BANDWIDTH_DEMAND 0
#define MAX_BANDWIDTH_DEMAND 5000

//�������·������
#define MAX_PATH_NUM 50000

//����·���ڵ��������1000
#define MAX_NODE_NUM_PER_PATH 1000

//���������ʱ��90s
#define MAX_RUN_TIME 90

//�������
#define INF 100000000

typedef  unsigned int uint;
typedef  unsigned char uchar;
typedef  pair<uint,uint> upair;//first������̾��룬second���涥��ı��
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

/*��·��Ϣ*/
typedef struct Edge{
	uint id;//�ߵ�id
	uint start_node;//��ʼ���
	uint end_node;//��ֹ���
	uint capacity;//����
	uint per_cost;//��λ�����ɱ�
	Edge(uint i, uint s, uint e, uint c, uint p ):
	id(i),start_node(s),end_node(e),capacity(c),per_cost(p){ }

}Edge;

/*�ڽӱ��еı���Ϣ  */
typedef struct Edge2
{
	uint end_node;
	uint capacity;
	uint per_cost;
	uint rev_id; //�յ㣬������ָ���������еģ������ã�����߱��(�ñ�ű�ʾ������u��v֮�����һ���ߣ���ô����u���ڽӱ��У���vΪ�յ�ı�����vΪ������ڽӱ��еĵڼ����ߣ�)
	Edge2(uint e,uint c,uint p,uint r) :
	end_node(e),capacity(c), per_cost(p), rev_id(r){}

}Edge2;

/*���ѽڵ���Ϣ*/
typedef struct ConsumNode{
	uint id;//���ѽڵ�id
	uint net_node_id;//��������ڵ�id
	uint bandwidth_demand;//��������
	ConsumNode(uint i,uint n,uint b):
	id(i),net_node_id(n),bandwidth_demand(b){}

}ConsumNode;

/*·����Ϣ*/
typedef struct Flow{
	vector<uint> route;//һ��·�� �ӷ�������������ڵ㵽�յ㣨���ѽڵ������ӵ�����ڵ㣩
	uint consum_id;//���ѽڵ�id
	uint bandwidth;//ռ�ô����С
}Flow;


/*����������*/
class Graph{
private:
	uint net_node_num;//����ڵ�����
	uint consum_node_num;//���ѽڵ�����
	uint link_num;//��·����
	uint per_server_cost;//ÿ����Ƶ�������ɱ�

	/*����Ϣ�����ѽڵ���Ϣ*/
	vector<Edge> edge_list;
	vector<ConsumNode>  consum_node_list;
	/*��������ʹ��۾���*/
	uint **p_capacity;
	uint **p_unit_cost;
	/*������ڵ������������*/
	vector<uint > net_node_flow;
	/*������ڵ�������vector<upair> ��pair<uint,uint>,first��ʾ�ڵ����flow��second��ʾ�ڵ�index*/
	vector<upair> sorted_net_node_flow;
	/*���ѽڵ�����������*/
	uint total_needed_flow;

//public://?????
	/*�ڽӱ�(�ڵ������Ϊ MAX_NETWORK_NODE_NUM+2 ) ����С���㷨����*/
	vector<Edge2 > adj_list[MAX_NETWORK_NODE_NUM+2];
	//vector<vector<Edge2>> adj_list;
public:
	/*����һ��Graph*/
	Graph();
	/*�������ͷ�graph��ռ�Ķ�̬������ڴ�*/
	~Graph();
	/*��ʼ������topo�ṹ*/
	void initial(char* topo[MAX_EDGE_NUM], int line_num);
	/*���һ��Edge2��adj_list��*/
	void addEdge2(uint from,uint to,uint cap,uint cost);
	/*��õ����������ɱ�*/
	uint getPerServerCost();
	/*��ȡ����ڵ���*/
	uint getNetNodeNum();
	/*��ȡ���ѽڵ���*/
	uint getConsumNodeNum();
	/*��ȡ��·��Ŀ*/
	uint getEdgeNum();
    uint getTotalNetNodeFlow();
	/*��ȡ���б�*/
	vector<Edge> & getEdgeList();
	/*��ȡ���ѽڵ��б�*/
	vector<ConsumNode> & getConsumNodeList();
	/*�������ѽڵ�����Ҫ��������*/
	void setTotalNeededFlow();
	/*��ȡ�ܹ���������*/
	uint getTotalNeededFlow();
	/*��������ڵ�id�õ����ѽڵ�id,��δ�ҵ��򷵻�-1*/
	int getConsumNodeId(uint net_node_id);
	/*���������ڵ�����������������������*/
	void setNetNodeFlow();
	/*��ȡָ������ڵ���������*/
	uint getNetNodeFlow(uint net_node_id);
	/*�Զ���upair<capacity,idx>������*/
	static bool netNodeFlowCompare(const upair & upair1,const upair & upair2);
	/*�Ը�����ڵ�������������С�������� */
	void sortNetNodeFlow();
	/*��ȡ�����������ڵ�������б�*/
	vector<upair>  getSortedNetNodeFlow();
	/*�����ڽӱ�,�������ΪĿ���ڽӱ� ���MCFF��*/
	void copyAdjList(vector<Edge2 > dest_adj_list[]);
	/*�����ڽӱ������ Server��ʹ�� ���ڼ������ѽڵ�����·��*/
	void getAdjList(vector<Edge2 > * &dest_adj_list);
	 uint getGegreeOfNetNode(uint net_node_id);//modify by hwj
};

/*��Դ�����С���ÿ����� min cost feasible flow*/
class MCFF{
private:

	uint vertex_num;//����ڵ�+����������
	uint distance[MAX_NETWORK_NODE_NUM+2];//�����㵽Դ�����̾���
	uint total_uint_cost[MAX_NETWORK_NODE_NUM+2];//������ƣ�һ��·���ϴ�Դ�㵽�յ��path �ϵ�uint cost ֮��
	uint pre_vertex[MAX_NETWORK_NODE_NUM+2];//���·�еĸ��ڵ�
	uint pre_edg2[MAX_NETWORK_NODE_NUM+2];//���·�еĸ���
	uint total_cost;//�ܵĳɱ�
	Graph *graph;//����topo
	vector<Edge2 > adj_list[MAX_NETWORK_NODE_NUM+2];//����ͼ���ڽӱ�ĸ��� ÿ����һ����Ҫ����һ��
	vector<Flow > best_paths;//��С�ɱ���������·���б�

	vector<vector<uint> > path_list;//·��

public:
	MCFF(Graph *g);
	~MCFF();
	/*���ڽӱ�����ӳ�����*/
	void addEdge2(uint from,uint to,uint cap,uint cost);
	/*���쳬��Դ�� �����յ��Ӧ�ı���ӵ��ڽӱ��� */
	void initAdjList(uint super_cource,
					 uint super_sink,
					 vector<uint> source,
					 vector<ConsumNode> consum_node_list);
	/*����Դ��  �����յ�  ��С���ÿ����� ���ĺ���*/
	long minCostFlowCore(uint super_source,
						 uint super_sink,
						 uint need_flow,
						 vector<Flow>& best_paths);
	/*����������λ�� �������������С�ɱ� ����Ӧ��·��*/
	long  getMinCostFlow(vector<uint> srever_id,vector<Flow>& best_paths );

	/*��ӡ��С�ɱ���·����Ŀ��·���б�*/
	void printFlowInfo(long total_cost,vector<Flow>& flow_list);


	///*������ �������Ļ*/
	//long  getMinCostFlow(vector<uint> srever_id);
	///*��ȡ·���б�*/
	//vector<vector<int> >& getPathList();
	///*��ӡ��С�ɱ���·����Ŀ��·���б�*/
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
