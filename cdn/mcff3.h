#ifndef __MCFF3_H__
#define __MCFF3_H__
#include "base.h"



typedef long long int excess_t; //excess of net node
typedef long long int cost_t;   //uint cost of arc

/* arc */
typedef struct Arc{
    long        resi_cap;     //residual capacity of a arc
    cost_t      cost;         //uint cost of a arc
    struct Node *p_head_node;  //heade node of arc
    Arc			*p_sisiter_arc; //reverse arc of a arc
}Arc;

/* node */
typedef  struct Node{
    excess_t         excess;         //the excess of node
    cost_t           price;          //distance from a sink
    Arc              *p_first_arc;   //first output arc of node
    Arc              *p_curr_arc;    //current output arc of node
    Arc              *p_suspend_arc; //suspended arc
    
    struct Node		 *p_queue_next;  //next node in push queue
    struct Node		 *p_buckets_next; //next node in bucket list
    struct Node		 *p_buckets_prev; //previous node in bucket list
    long             rank;            //bucket number
    long             inp;             //auxilary field
} Node;


/*node bucket*/
typedef struct NodeBucket{
    Node             *p_first_node;   //1st node with positive excess or simply 1st node in the buket
} NodeBucket;



/*MCFF3: min cost feasible flow*/
class MCFF3{
private:
    Graph *graph;
    bool is_feasible;//solution is feasible or not
    bool is_calc_path; //calculate path
    vector<Edge> super_edge_list;// the edge of super source and sink
    vector<Edge2> adj_list[MAX_NETWORK_NODE_NUM+2];//for calcaulate path
    uint distance[MAX_NETWORK_NODE_NUM+2];//distance to source node
    uint pre_vertex[MAX_NETWORK_NODE_NUM+2];//parent node array
    uint pre_edg2[MAX_NETWORK_NODE_NUM+2];//parent edge array
    
    long n;	//num of net nodes
    long m; //num of arcs
    uint min_node_id;
    
    Node *p_nodes; // node list
    Node *p_end_node; //next after last node
    Node *p_queue_first;  // first node in push queue
    Node *p_queue_last;   // last node in push queue
    
    Arc  *p_arcs;  //arc list
    Arc	 *p_end_arc;  //next after last arc
    
    long *p_cap;//capacities array
    NodeBucket  *p_buckets;   //bucket list
    NodeBucket	*p_last_bucket; // last bucket
    long		 linf;    //number of p_last_bucket + 1
    
    int time_for_price_in;
    cost_t  epsilon;    // quality measure
    cost_t	cost_mltiplier;     // cost multiplier - number of nodes  + 1
    cost_t  price_min;  //lowest bound for prices
    cost_t	muti_max_cost;    //multiplied max cost
    cost_t  max_arc_cost;//max arc cost
    double  f_scale;     //scale factor
    double  cut_off_factor;  //multiplier to produce cut_on and cut_off from n and epsilon
    double	cut_on;          // the bound for returning suspended arcs
    double	cut_off;        //the bound for suspending arcs
    
    excess_t total_excess;  //total excess
    
    long	n_rel;     //number of relabels from last price update
    long	n_ref;     //current number of refines
    long	n_src;     //current number of nodes with excess
    
    int     flag_price ;	//if = 1 - signal to start price-in ASAP - maybe there is infeasibility because of susoended arcs
    int	    flag_updt ;		//if = 1 - update failed some sources are unreachable: either the problem is unfeasible or
    //you have to return  suspended arcs
    
    int     snc_max;   //maximal number of cycles cancelled during price refine
    
    Arc     d_arc;     //dummy arc - for technical reasons
    
    Node    d_node;    //dummy node - for technical reasons
    Node	*dummy_node;  //the address of d_node
    
    //Node	dnd, *dnode;//global variable  不能放在类里面 放在mcff3_macro.h
    
    
    long n_push     ,
    n_relabel   ,
    n_discharge ,
    n_refine    ,
    n_update    ,
    n_scan      ,
    n_prscan    ,
    n_prscan1   ,
    n_prscan2   ,
    n_bad_pricein,
    n_bad_relabel,
    n_prefine   ;  // record the run times of functions
    
    
    
public:
    MCFF3(Graph *g);
    
    ~MCFF3();
    
    void initFuncCount();
    
    void addEdge2(uint from,uint to,uint cap,uint cost);
    
    void createSuperEdgeList(vector<uint> server_id,
                             uint super_source,
                             uint super_sink);
    
    int initNetworkTopo(uint super_source,
                        uint super_sink);
    
    void initCostScale(long scale_fac);
    
    void upNodeScan(Node *pni);
    
    void priceUpdate();
    
    int relabel(Node *pi);
    
    void discharge(Node *pi);
    
    int priceIn();
    
    void refine();
    
    int priceRefine();
    
    void computePrices();
    
    void priceOut();
    
    int updateEpsilon();
    
    int checkCS();
    
    int checkEpsOpt();
    
    long finishUp();
    
    void initSolution();
    
    long costScaleCore(long scale_fac);
    
    
    bool isFeasible(vector<uint> selected_server_id);
    
    long minCostFlowCore(uint super_source,
                         uint super_sink,
                         uint need_flow,
                         vector<Flow>& best_paths);
    
    long getMinCost(vector<uint> selected_server_id);
    
    long getMinCostFlow(vector<uint> best_server_id,
                        vector<Flow>& best_paths);
    
    void printFlowInfo(long total_cost,
                       vector<Flow>& best_paths);
    
    
};







#endif
