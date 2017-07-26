
#include "mcff3_macro.h"
#include "mcff3.h"

MCFF3::MCFF3(Graph *g){
    if(g){
        graph=g;
    }else{
        cout<<"MCFF3() failed"<<endl;
    }
    initFuncCount();
    is_feasible=true;
    is_calc_path=false;
    
}

MCFF3::~MCFF3(){
    
}

void MCFF3::initFuncCount(){
    n_push      =0;
    n_relabel   =0;
    n_discharge =0;
    n_refine    =0;
    n_update    =0;
    n_scan      =0;
    n_prscan    =0;
    n_prscan1   =0;
    n_prscan2   =0;
    n_bad_pricein = 0;
    n_bad_relabel = 0;
    n_prefine   =0;
    flag_price =0;
    flag_updt=0 ;
}

void MCFF3::addEdge2(uint from,uint to,uint cap,uint cost){
    adj_list[from].push_back(Edge2(to,cap,cost,adj_list[to].size()));
    adj_list[to].push_back(Edge2(from,0,-cost,adj_list[from].size()-1));
}

void MCFF3::createSuperEdgeList(vector<uint> server_id,
                                uint super_source,
                                uint super_sink){
    super_edge_list.clear();
    uint egde_num=graph->getEdgeNum();
    
    for(int i=0;i<server_id.size();i++){
        super_edge_list.push_back(Edge(egde_num++,super_source,server_id[i],INF,0));
    }
    
    vector<ConsumNode> consum_node_list=graph->getConsumNodeList();
    for(int i=0;i<consum_node_list.size();i++){
        super_edge_list.push_back(Edge(egde_num++,consum_node_list[i].net_node_id,
                                       super_sink,consum_node_list[i].bandwidth_demand,0));
    }
}

int MCFF3::initNetworkTopo(uint super_source,
                           uint super_sink){
    
    /* temporary variables carrying no of nodes */
    uint  head, tail,i,j,k, min_node_id, max_node_id;
    /* internal array for holding -node degree, -position of the first outgoing arc*/
    long *arc_first;
    /* internal array: tails(no of node) of the arcs   */
    long *arc_tail;
    /*temporary pointers to net node structure,pointer i,j,head,du to node*/
    Node *pni,*pnj,*pnhead,*pndu;
    /*pointers to Arc*/
    Arc *p_cur_arc,*p_new_arc,*p_tmp_arc;
    /*supply and demand*/
    excess_t total_supply,total_demand,cap_in_sum,cap_out_sum;
    /*cur_pos:2*(no of arc),low:lower bound of cap,acap:cap of arc, */
    long cur_pos=0,low=0,acap=0;
    /*temporary variables carrying no of nodes*/
    long arc_num,arc_new_num,last;
    /*arc cost*/
    cost_t cost;
    /*local variable*/
    long n,m;
    long *p_cap;
    Node *p_nodes;
    Arc *p_arcs;
    cost_t  max_arc_cost;
    
    /*add super source and super sink*/
    n=graph->getNetNodeNum()+2;
    /*2*graph->getEdgeNum() for undirected graph*/
    m=2*graph->getEdgeNum()+super_edge_list.size();
    
    /*allocating memory for  'nodes', 'arcs' and internal arrays */
    p_nodes = (Node*) calloc (n+2,sizeof(Node));
    p_arcs  = (Arc*)  calloc (2*m+1,sizeof(Arc));
    p_cap   = (long*) calloc (2*m,sizeof(long));
    arc_first= (long*) calloc (n+2,sizeof(long));
    arc_tail = (long*) calloc (2*m,sizeof(long));
    
    /*initial excess of each node*/
    for(pni=p_nodes;pni<=p_nodes+n;pni++){
        pni->excess=0;
    }
    
    /*initial local variable */
    p_cur_arc=p_arcs;
    max_node_id=0;
    min_node_id=n;
    max_arc_cost=0;
    total_supply=total_demand=0;
    
    for(pndu=p_nodes;pndu<p_nodes+n;pndu++){
        pndu->excess=0;
    }
    
    excess_t total_needed_flow=graph->getTotalNeededFlow();
    (p_nodes + super_source)->excess =total_needed_flow;
    (p_nodes + super_sink)->excess = -total_needed_flow;
    total_supply=total_demand= total_needed_flow;
    
    /*process edge_list arc info*/
    vector<Edge> edge_list=graph->getEdgeList();
    for( i=0;i<edge_list.size();i++){
        Edge edge=edge_list[i];
        /*undirected graph */
        for( j=0;j<2;j++){
            if(j==0){
                tail=edge.start_node;
                head=edge.end_node;
            }else{//brother arc
                tail=edge.end_node;
                head=edge.start_node;
            }
            acap=edge.capacity;
            cost=edge.per_cost;
            /*no of arcs incident to node i is placed in arc_first[i+1] */
            arc_first[tail+1]++;
            arc_first[head+1]++;
            
            pni= p_nodes+tail;//point to start(tail) node of acr
            pnj= p_nodes+head;//point to end(head) node of rac
            
            /* storing information about the arc*/
            arc_tail[cur_pos]= tail;//no of tail node of arc
            arc_tail[cur_pos+1] = head;//no of head node of arc
            
            p_cur_arc ->p_head_node=pnj;
            p_cur_arc ->resi_cap=acap-low;
            /*storing capacity of arcs*/
            p_cap[cur_pos]=acap;
            p_cur_arc->cost=cost;
            p_cur_arc->p_sisiter_arc=p_cur_arc+1;
            
            (p_cur_arc+1)->p_head_node= p_nodes + tail;//reverse arc
            (p_cur_arc+1)->resi_cap=0;
            p_cap[cur_pos+1]= 0;
            (p_cur_arc+1)->cost= -cost;
            (p_cur_arc+1)->p_sisiter_arc= p_cur_arc;
            
            pni->excess -= low;
            pnj->excess += low;
            
            /* searching for minimum and maximum node */
            if (head < min_node_id )
                min_node_id = head;
            if (tail < min_node_id )
                min_node_id = tail;
            if (head > max_node_id )
                max_node_id = head;
            if (tail > max_node_id )
                max_node_id = tail;
            
            if (cost<0)
                cost=-cost;
            if (cost>max_arc_cost && acap > 0 )
                max_arc_cost = cost;//updaate maximum arc cost
            
            p_cur_arc+=2;
            cur_pos+=2;
        }
    }
    
    /*process super_edge_list arc info*/
    for( i=0;i<super_edge_list.size();i++){
        Edge edge=super_edge_list[i];
        
        tail=edge.start_node;
        head=edge.end_node;
        acap=edge.capacity;
        cost=edge.per_cost;
        /*no of arcs incident to node i is placed in arc_first[i+1] */
        arc_first[tail+1]++;
        arc_first[head+1]++;
        
        pni= p_nodes+tail;//point to start(tail) node of acr
        pnj= p_nodes+head;//point to end(head) node of rac
        
        /* storing information about the arc*/
        arc_tail[cur_pos]= tail;//no of tail node of arc
        arc_tail[cur_pos+1] = head;//no of head node of arc
        
        p_cur_arc ->p_head_node=pnj;
        p_cur_arc ->resi_cap=acap-low;
        /*storing capacity of arcs*/
        p_cap[cur_pos]=acap;
        p_cur_arc->cost=cost;
        p_cur_arc->p_sisiter_arc=p_cur_arc+1;
        
        
        (p_cur_arc+1)->p_head_node= p_nodes + tail;//reverse arc
        (p_cur_arc+1)->resi_cap=0;
        p_cap[cur_pos+1]= 0;
        (p_cur_arc+1)->cost= -cost;
        (p_cur_arc+1)->p_sisiter_arc= p_cur_arc;
        
        pni->excess -= low;
        pnj->excess += low;
        
        /* searching for minimum and maximum node */
        if (head < min_node_id )
            min_node_id = head;
        if (tail < min_node_id )
            min_node_id = tail;
        if (head > max_node_id )
            max_node_id = head;
        if (tail > max_node_id )
            max_node_id = tail;
        
        if (cost<0)
            cost=-cost;
        if (cost>max_arc_cost && acap > 0 )
            max_arc_cost = cost;//updaate maximum arc cost
        
        p_cur_arc+=2;
        cur_pos+=2;
    }
    
    /*ordering arcs */
    (p_nodes+min_node_id)->p_first_arc=p_arcs;//first arc from the first node
    
    /*before below loop arc_first[i+1] is the number of arcs outgoing from i;
     after this loop arc_first[i] is the position of the first  outgoing from node i arcs after they would be ordered;
     this value is transformed to pointer and written to node.first[i]*/
    
    for( i=min_node_id+1;i<=max_node_id+1;i++){
        arc_first[i] += arc_first[i-1];
        (p_nodes+i)->p_first_arc=p_arcs+arc_first[i];
    }
    
    /* scanning all the nodes  exept the last*/
    for ( i = min_node_id; i < max_node_id; i ++ ) {
        last = ((p_nodes+i+1)-> p_first_arc )-p_arcs;
        /* arcs outgoing from i must be cited
         from position arc_first[i] to the position
         equal to initial value of arc_first[i+1]-1  */
        
        for ( arc_num = arc_first[i]; arc_num < last; arc_num ++ ){
            tail = arc_tail[arc_num];
            /* the arc no  arc_num  is not in place because arc cited here
             must go out from i;
             we'll put it to its place and continue this process
             until an arc in this position would go out from i */
            while ( tail != i ){
                arc_new_num = arc_first[tail];
                p_cur_arc = p_arcs+arc_num;
                p_new_arc= p_arcs+arc_new_num;
                
                /* p_cur_arc must be cited in the position arc_new
                 swapping these arcs:                                 */
                
                pnhead = p_new_arc->p_head_node;
                p_new_arc->p_head_node = p_cur_arc->p_head_node;
                p_cur_arc->p_head_node= pnhead;
                
                acap= p_cap[arc_new_num];
                p_cap[arc_new_num] = p_cap[arc_num];
                p_cap[arc_num] = acap;
                
                acap= p_new_arc-> resi_cap;
                p_new_arc->resi_cap= p_cur_arc-> resi_cap;
                p_cur_arc->resi_cap = acap;
                
                cost= p_new_arc-> cost;
                p_new_arc->cost= p_cur_arc-> cost;
                p_cur_arc->cost= cost;
                
                if ( p_new_arc != p_cur_arc -> p_sisiter_arc ){
                    p_tmp_arc= p_new_arc -> p_sisiter_arc;
                    p_new_arc->p_sisiter_arc=p_cur_arc -> p_sisiter_arc;
                    p_cur_arc->p_sisiter_arc=p_tmp_arc;
                    (p_cur_arc->p_sisiter_arc)->p_sisiter_arc= p_cur_arc;
                    (p_new_arc->p_sisiter_arc)->p_sisiter_arc= p_new_arc;
                }
                
                arc_tail[arc_num]=arc_tail[arc_new_num];
                arc_tail[arc_new_num]=tail;
                
                /* we increase arc_first[tail]  */
                arc_first[tail] ++ ;
                
                tail = arc_tail[arc_num];
            }
        }
        /* all arcs outgoing from  i  are in place */
    }
    
    
    
    /* -----------------------  arcs are ordered  ------------------------- */
    /*------------ testing network for possible excess overflow ---------*/
    for (pndu = p_nodes + min_node_id; pndu <= p_nodes + max_node_id; pndu ++ ){
        cap_in_sum = (pndu-> excess);
        cap_out_sum = -( pndu->excess);
        for ( p_cur_arc = pndu->p_first_arc; p_cur_arc != (pndu+1) -> p_first_arc; p_cur_arc ++ ){
            arc_num = p_cur_arc - p_arcs;
            if ( p_cap[arc_num]>0 ) cap_out_sum += p_cap[arc_num];
            if ( p_cap[arc_num]== 0 )
                cap_in_sum += p_cap[( p_cur_arc -> p_sisiter_arc )-p_arcs];
        }
    }
    
    this->n=max_node_id-min_node_id+1;
    this->m=m;
    this->min_node_id=min_node_id;
    this->p_nodes=p_nodes;
    this->p_arcs=p_arcs;
    this->max_arc_cost=max_arc_cost;
    this->p_cap=p_cap;
    
    /* free internal memory */
    free ( arc_first );
    free ( arc_tail );
    
    return 0;
}

void MCFF3::initCostScale(long scale_fac){
    Node *pn;//point to current node
    Arc *pa,*pa_stop;//point to current arc
    long cap_diff=0;
    NodeBucket *pb;
    
    /*set MCFF3 member variable*/
    p_end_node=p_nodes+n;
    m=2*m;//!!!!!
    p_end_arc=p_arcs+m;
    f_scale=scale_fac;
    cost_mltiplier=n+1;
    
    for(pn=p_nodes;pn!=p_end_node;pn++){
        pn->price=0;
        pn->p_suspend_arc=pn->p_first_arc;
        pn->p_queue_next=p_end_node;
    }
    
    p_end_node->p_first_arc=p_end_node->p_suspend_arc=p_end_arc;
    
    for(pn=p_nodes;pn!=p_end_node;pn++){
        /*for active arcs a from node i*/
        pa=pn->p_first_arc;
        pa_stop=(pn+1)->p_suspend_arc;
        for(;pa!=pa_stop;pa++){
            if(pa->cost<0){
                if((cap_diff=pa->resi_cap)>0){
                    INCREASE_FLOW(pn,pa->p_head_node,pa,cap_diff);
                }
            }
        }
    }
    
    for(pa=p_arcs;pa!=p_end_arc;pa++){
        pa->cost*=cost_mltiplier;
    }
    if ((double)max_arc_cost *(double)cost_mltiplier > MAX_64)
        fprintf(stdin, "Warning: arc lengths too large, overflow possible\n");
    
    /*set MCFF3 member variable*/
    muti_max_cost = max_arc_cost * cost_mltiplier;
    linf=(long)(cost_mltiplier * ceil(f_scale) +2);
    p_buckets=(NodeBucket *)calloc(linf,sizeof(NodeBucket));
    
    if(NULL==p_buckets){
        /* modify by wjs 0326 */
        /*set MCFF3 member variable*/
        is_feasible=false;
        goto end;
    }
    /*set MCFF3 member variable*/
    p_last_bucket=p_buckets+linf;
    dnode=&dnd;//???
    
    for(pb=p_buckets;pb!=p_last_bucket;pb++){
        RESET_BUCKET(pb);
    }
    
    /*set MCFF3 member variable*/
    epsilon=muti_max_cost;
    if (epsilon<1)
        epsilon = 1;
    price_min= -PRICE_MAX;
    cut_off_factor=CUT_OFF_COEF*pow((double)n, CUT_OFF_POWER);
    cut_off_factor=MAX(cut_off_factor, CUT_OFF_MIN);
    n_ref= 0;
    flag_price=0;
    dummy_node=&d_node;//????
    p_queue_first=NULL;
    
end:
    return;
}

void MCFF3::upNodeScan(Node *pni){
    Node *pnj;                     /* opposite node */
    Arc  *pa,                     /* ( i, j ) */
    *pa_stop,                /* first arc from the next node */
    *pa_r;                    /* ( j, i ) */
    NodeBucket *pb_old, *pb_new;   /* old bucket contained j, new bucket for j */
    long i_rank,j_rank,j_new_rank; /* ranks of nodes */
    cost_t redu_cost,                    /* reduced cost of (j,i) */
    rank_diff;                     /* rank difference */
    
    n_scan ++;//
    i_rank =pni->rank;
    
    /*for actives pa from node pni*/
    pa=pni->p_first_arc;
    pa_stop=(pni+1)->p_suspend_arc;
    for(;pa!=pa_stop;pa++){
        pa_r=pa->p_sisiter_arc;
        if(OPEN(pa_r)){
            pnj=pa->p_head_node;
            j_rank=pnj->rank;
            if(j_rank>i_rank){
                if((redu_cost= REDUCED_COST(pnj, pni, pa_r)) <0){
                    j_new_rank=i_rank;
                }else{
                    rank_diff=redu_cost/epsilon;
                    j_new_rank=(rank_diff<linf)?i_rank+(long)rank_diff+1:linf;
                }
                
                if(j_rank>j_new_rank){
                    pnj->rank=j_new_rank;
                    pnj->p_curr_arc=pa_r;
                    if(j_rank<linf){
                        pb_old=p_buckets+j_rank;
                        REMOVE_FROM_BUCKET(pnj,pb_old);
                    }
                    pb_new=p_buckets+j_new_rank;
                    INSERT_TO_BUCKET(pnj,pb_new);
                }
            }
        }
    }
    pni->price-=i_rank*epsilon;
    pni->rank=-1;
}

void MCFF3::priceUpdate(){
    register Node   *pi;
    excess_t remain;   /* total excess of unscanned nodes with  positive excess */
    NodeBucket *pb;    /* current bucket */
    cost_t dp;     /* amount to be subtracted from prices */
    
    n_update ++;
    for(pi=p_nodes;pi!=p_end_node;pi++){
        if(pi->excess<0){
            INSERT_TO_BUCKET(pi,p_buckets);
            pi->rank=0;
        }else{
            pi->rank=linf;
        }
    }
    
    remain=total_excess;
    if ( remain < 0.5 )
        return;
    
    for(pb=p_buckets;pb!=p_last_bucket;pb++){
        while(NONEMPTY_BUCKET (pb)){
            GET_FROM_BUCKET ( pi, pb )
            upNodeScan(pi);
            if(pi->excess>0){
                remain-=(pi->excess);
                if(remain<=0)
                    break;
            }
        }
        if(remain<=0)
            break;
    }
    
    if(remain>0.5)
        flag_updt=1;
    dp=(pb-p_buckets)*epsilon;
    
    for(pi=p_nodes;pi!=p_end_node;pi++){
        if(pi->rank>=0){
            if(pi->rank<linf)
                REMOVE_FROM_BUCKET(pi,(p_buckets+pi->rank));
            if(pi->price>price_min)
                pi->price-=dp;
        }
    }
}

int MCFF3::relabel(Node *pi){
    register Arc	*pa,       /* current arc from  i  */
    *pa_stop,  /* first arc from the next node */
    *pa_max;   /* arc  which provides maximum price */
    register cost_t cur_max_price,    /* current maximal price */
			 ni_price,  /* price of node  i */
			 dp;
    cur_max_price=price_min;
    ni_price=pi->price;
    
    pa_max=NULL;
    
    for (pa=pi->p_curr_arc + 1, pa_stop=(pi+1)->p_suspend_arc;pa!= pa_stop;pa++){
        if (OPEN(pa)&& ((dp = ((pa->p_head_node)->price)-(pa->cost)) > cur_max_price)){
            if ( ni_price < dp ){
                pi -> p_curr_arc = pa;
                return 1;
            }
            cur_max_price = dp;
            pa_max = pa;
        }
    }
    
    
    for(pa= pi->p_first_arc, pa_stop= (pi->p_curr_arc) + 1;pa!= pa_stop;pa ++){
        if (OPEN(pa)&&((dp=((pa->p_head_node)->price)-(pa->cost))>cur_max_price)){
            if (ni_price< dp ){
                pi->p_curr_arc = pa;
                return 1;
            }
            cur_max_price = dp;
            pa_max = pa;
        }
    }
    
    if (cur_max_price != price_min ){
        pi->price = cur_max_price - epsilon;
        pi->p_curr_arc =pa_max;
    }else{ /* node can't be relabelled */
        if (pi->p_suspend_arc==pi->p_first_arc ){
            if (pi->excess== 0 ){
                pi->price=price_min;
            }else{
                if (n_ref == 1){
                    /*wjs modify------------*/
                    is_feasible=false;
                    goto end;
                }
                else{
                    /*wjs modify------------*/
                    is_feasible=false;
                    goto end;
                }
            }
        }else {/* node can't be relabelled because of suspended arcs */
            flag_price = 1;
        }
    }
    
    n_relabel ++;
    n_rel ++;
    
end:
    return 0;
}

void MCFF3::discharge(Node *pi){
    register Arc  *pa;       /* an arc from  i  */
    register Node *pj;       /* head of  a  */
    register long df;       /* amoumt of flow to be pushed through  a  */
    excess_t j_exc;             /* former excess of  j  */
    
    n_discharge ++;
    
    pa = pi -> p_curr_arc;
    pj = pa -> p_head_node;
    
    if ( !ADMISSIBLE ( pi, pj, pa ) ) {
        relabel ( pi );
        pa = pi -> p_curr_arc;
        pj = pa -> p_head_node;
    }
    
    while ( 1 ){
        j_exc = pj -> excess;
        
        if ( j_exc >= 0 ){
            df = MIN( pi -> excess, pa -> resi_cap );
            if (j_exc == 0) n_src++;
            INCREASE_FLOW( pi, pj, pa, df );
            n_push ++;
            
            if ( OUT_OF_EXCESS_Q (pj) ){
                INSERT_TO_EXCESS_Q (pj);
            }
        }else{ /* j_exc < 0 */
            df = MIN( pi-> excess, pa->resi_cap );
            INCREASE_FLOW( pi, pj, pa, df );
            n_push ++;
            
            if ( pj -> excess >= 0 ){
                if ( pj -> excess > 0 ){
                    n_src++;
                    relabel (pj);
                    INSERT_TO_EXCESS_Q (pj);
                }
                total_excess += j_exc;
            }
            else
                total_excess -= df;
            
        }
        
        if (pi -> excess <= 0)
            n_src--;
        if ( pi -> excess <= 0 || flag_price ) break;
        
        relabel ( pi );
        
        pa = pi -> p_curr_arc;
        pj = pa -> p_head_node;
    }
    
    pi -> p_curr_arc = pa;
    
}

int MCFF3::priceIn(){
    Node *pi, *pj;                  /* current node */
    Arc *pa,                   /* current arc from i */
    *pa_stop,              /* first arc from the next node */
    *pb,                   /* arc to be exchanged with suspended */
    *pa_r,                  /* opposite to  a  */
    *pb_r;                  /* opposite to  b  */
    
    cost_t   redu_cost;      /* reduced cost */
    int     n_in_bad,           /* number of priced_in arcs with
                                 negative reduced cost */
    bad_found;          /* if 1 we are at the second scan
                         if 0 we are at the first scan */
    excess_t  ni_exc,                /* excess of  i  */
    df;                   /* an amount to increase flow */
    
    bad_found = 0;
    n_in_bad = 0;
    
restart:
    for(pi=p_nodes;pi!=p_end_node;pi++){
        for ( pa = ( pi -> p_first_arc ) - 1, pa_stop = ( pi -> p_suspend_arc ) - 1; pa != pa_stop; pa -- ){
            redu_cost = REDUCED_COST ( pi, pa -> p_head_node, pa );
            
            if ( (redu_cost < 0) && ( pa -> resi_cap > 0) ){ /* bad case */
                if ( bad_found == 0 ){
                    bad_found = 1;
                    UPDATE_CUT_OFF;
                    goto restart;
                }
                
                df = pa -> resi_cap;
                INCREASE_FLOW( pi, pa -> p_head_node, pa, df );
                
                pa_r = pa -> p_sisiter_arc;
                pj  = pa -> p_head_node;
                
                pb = -- ( pi -> p_first_arc );
                EXCHANGE ( pa, pb );
                
                if ( SUSPENDED ( pj, pa_r ) ){
                    pb_r = -- ( pj -> p_first_arc );
                    EXCHANGE ( pa_r, pb_r );
                }
                
                n_in_bad ++;
            }else if ((redu_cost < cut_on ) && ( redu_cost > -cut_on ) ){
                pi->p_first_arc--;
                pb = pi -> p_first_arc;
                EXCHANGE ( pa, pb );
            }
        }
    }
    
    if ( n_in_bad != 0 ){
        n_bad_pricein ++;
        /* recalculating excess queue */
        total_excess = 0;
        n_src=0;
        RESET_EXCESS_Q;
        
        for(pi=p_nodes;pi!=p_end_node;pi++) {
            pi -> p_curr_arc = pi -> p_first_arc;
            ni_exc = pi -> excess;
            if ( ni_exc > 0 ){ /* i  is a source */
                total_excess += ni_exc;
                n_src++;
                INSERT_TO_EXCESS_Q ( pi );
            }
        }
        
        INSERT_TO_EXCESS_Q ( dummy_node );
    }
    
    if (time_for_price_in == TIME_FOR_PRICE_IN2)
        time_for_price_in = TIME_FOR_PRICE_IN3;
    
    if (time_for_price_in == TIME_FOR_PRICE_IN1)
        time_for_price_in = TIME_FOR_PRICE_IN2;
    
    return ( n_in_bad );
    
end:
    return 0;
}

void MCFF3::refine(){
    Node     *pi;      /* current node */
    excess_t ni_exc;   /* excess of  i  */
    long   np, nr, ns;  /* variables for additional print */
    int    pr_in_int;   /* current number of updates between price_in */
    
    np = n_push;
    nr = n_relabel;
    ns = n_scan;
    
    n_refine ++;
    n_ref ++;
    n_rel = 0;
    pr_in_int = 0;
    
    /* initialize */
    
    total_excess = 0;
    n_src=0;
    RESET_EXCESS_Q
    time_for_price_in = TIME_FOR_PRICE_IN1;
    
    for(pi=p_nodes;pi!=p_end_node;pi++) {
        pi -> p_curr_arc = pi -> p_first_arc;
        ni_exc = pi -> excess;
        if ( ni_exc > 0 ){ /* i  is a source */
            total_excess += ni_exc;
            n_src++;
            INSERT_TO_EXCESS_Q ( pi )
        }
    }
    
    if ( total_excess <= 0 )
        return;
    
    /* main loop */
    while ( 1 ){
        if ( EMPTY_EXCESS_Q ){
            if ( n_ref > PRICE_OUT_START ) {
                pr_in_int = 0;
                priceIn ();
            }
            
            if ( EMPTY_EXCESS_Q ) break;
        }
        
        REMOVE_FROM_EXCESS_Q ( pi );
        
        /* push all excess out of i */
        
        if ( pi -> excess > 0 ){
            discharge ( pi );
            
            if ( TIME_FOR_UPDATE || flag_price ){
                if ( pi -> excess > 0 ){
                    INSERT_TO_EXCESS_Q ( pi );
                }
                
                if ( flag_price && ( n_ref > PRICE_OUT_START ) ){
                    pr_in_int = 0;
                    priceIn ();
                    flag_price = 0;
                }
                
                priceUpdate();
                
                while ( flag_updt ){
                    if ( n_ref == 1 ){
                        /*wjs modify------------*/
                        is_feasible=false;
                        flag_updt = 0;
                        goto end;
                        /*wjs modify------------*/
                    }
                    else{
                        flag_updt = 0;
                        UPDATE_CUT_OFF;
                        n_bad_relabel++;
                        pr_in_int = 0;
                        priceIn ();
                        priceUpdate ();
                    }
                }
                
                n_rel = 0;
                
                if ( n_ref > PRICE_OUT_START && (pr_in_int ++ > time_for_price_in) ){
                    pr_in_int = 0;
                    priceIn ();
                }
                
            } /* time for update */
        }
    } /* end of main loop */
    
end:
    return;
}

int MCFF3::priceRefine(){
    Node   *pi,              /* current node */
    *pj,              /* opposite node */
    *pir,             /* nodes for passing over the negative cycle */
    *pis;
    Arc     *pa,              /* arc (i,j) */
    *pa_stop,         /* first arc from the next node */
    *par;
    
    long   bmax;            /* number of farest nonempty bucket */
    long   i_rank,          /* rank of node i */
    j_rank,          /* rank of node j */
    j_new_rank;      /* new rank of node j */
    NodeBucket  *pb,              /* current bucket */
				*pb_old,          /* old and new buckets of current node */
				*pb_new;
    cost_t redu_cost,              /* reduced cost of a */
    rank_diff,              /* ranks difference */
    dp;
    int    cc;              /* return code: 1 - flow is epsilon optimal
                             0 - refine is needed        */
    long   df;              /* cycle capacity */
    
    int     nnc,             /* number of negative cycles cancelled duringone iteration */
    snc;             /* total number of negative cycle cancelled */
    
    n_prefine ++;
    
    cc=1;
    snc=0;
    
    snc_max = ( n_ref >= START_CYCLE_CANCEL ) ? MAX_CYCLES_CANCELLED: 0;
    
    /* main loop */
    
    while (1){ /* while negative cycle is found or eps-optimal solution is constructed */
        nnc=0;
        
        for(pi=p_nodes;pi!=p_end_node;pi++) {
            pi -> rank    = 0;
            pi -> inp     = WHITE;
            pi -> p_curr_arc = pi -> p_first_arc;
        }
        
        RESET_STACKQ
        
        for(pi=p_nodes;pi!=p_end_node;pi++) {
            if ( pi -> inp == BLACK ) continue;
            
            pi -> p_buckets_next = NULL;
            
            /* deapth first search */
            while ( 1 ){
                pi -> inp = GREY;
                
                /* scanning arcs from node i starting from current */
                
                pa_stop=(pi+1)->p_suspend_arc;
                for(pa=pi->p_curr_arc;pa!=pa_stop;pa++) {
                    if ( OPEN ( pa ) ){
                        pj = pa -> p_head_node;
                        if ( REDUCED_COST ( pi, pj, pa ) < 0 ){
                            if ( pj -> inp == WHITE ){ /* fresh node  - step forward */
                                pi -> p_curr_arc = pa;
                                pj -> p_buckets_next  = pi;
                                pi = pj;
                                pa = pj -> p_curr_arc;
                                pa_stop = (pj+1) -> p_suspend_arc;
                                break;
                            }
                            
                            if ( pj -> inp == GREY ){ /* cycle detected */
                                cc = 0;
                                nnc++;
                                
                                pi -> p_curr_arc = pa;
                                pis = pir = pi;
                                df = MAX_32;
                                
                                while ( 1 ){
                                    par = pir -> p_curr_arc;
                                    if ( par -> resi_cap <= df ){
                                        df = par -> resi_cap;
                                        pis = pir;
                                    }
                                    if ( pir == pj ) break;
                                    pir = pir -> p_buckets_next;
                                }
                                
                                
                                pir = pi;
                                
                                while ( 1 ){
                                    par = pir -> p_curr_arc;
                                    INCREASE_FLOW( pir, par -> p_head_node, par, df);
                                    
                                    if ( pir == pj ) break;
                                    pir = pir -> p_buckets_next;
                                }
                                
                                
                                if ( pis != pi ){
                                    for ( pir = pi; pir != pis; pir = pir -> p_buckets_next )
                                        pir -> inp = WHITE;
                                    
                                    pi = pis;
                                    pa = (pis -> p_curr_arc) + 1;
                                    pa_stop = (pis+1) -> p_suspend_arc;
                                    break;
                                }
                                
                            }
                        }
                        /* if j-color is BLACK - continue search from i */
                    }
                } /* all arcs from i are scanned */
                
                if ( pa == pa_stop ){
                    /* step back */
                    pi -> inp = BLACK;
                    n_prscan1++;
                    pj = pi -> p_buckets_next;
                    STACKQ_PUSH ( pi );
                    
                    if ( pj == NULL ) break;
                    pi = pj;
                    pi -> p_curr_arc ++;
                }
                
            } /* end of deapth first search */
        } /* all nodes are scanned */
        
        /* no negative cycle */
        /* computing longest paths with eps-precision */
        
        
        snc += nnc;
        
        if ( snc<snc_max ) cc = 1;
        
        if ( cc == 0 ) break;
        
        bmax = 0;
        
        while ( NONEMPTY_STACKQ ){
            n_prscan2++;
            STACKQ_POP ( pi );
            i_rank = pi -> rank;
            pa_stop=(pi+1)->p_suspend_arc;
            for(pa=pi->p_first_arc;pa!=pa_stop;pa++) {
                if ( OPEN ( pa ) ){
                    pj  = pa -> p_head_node;
                    redu_cost = REDUCED_COST ( pi, pj, pa );
                    
                    
                    if ( redu_cost < 0 ) /* admissible arc */{
                        rank_diff = (cost_t) (( - redu_cost - 0.5 ) / epsilon);
                        if (( j_rank = rank_diff + i_rank ) < linf ){
                            if ( j_rank > pj -> rank )
                                pj -> rank = j_rank;
                        }
                    }
                }
            } /* all arcs from i are scanned */
            
            if ( i_rank > 0 ){
                if ( i_rank > bmax ) bmax = i_rank;
                pb = p_buckets + i_rank;
                INSERT_TO_BUCKET( pi, pb );
            }
        } /* end of while-cycle: all nodes are scanned
           - longest distancess are computed */
        
        
        if ( bmax == 0 ){  /* preflow is eps-optimal */
            break;
        }
        
        for ( pb = p_buckets + bmax; pb != p_buckets; pb -- ){
            i_rank = pb - p_buckets;
            dp     = i_rank * epsilon;
            
            while ( NONEMPTY_BUCKET( pb ) ){
                GET_FROM_BUCKET ( pi, pb );
                
                n_prscan++;
                pa_stop=(pi+1)->p_suspend_arc;
                for(pa=pi->p_first_arc;pa!=pa_stop;pa++){
                    if ( OPEN ( pa ) ){
                        pj = pa -> p_head_node;
                        j_rank = pj -> rank;
                        if ( j_rank < i_rank ){
                            redu_cost = REDUCED_COST ( pi, pj, pa );
                            
                            if ( redu_cost < 0 ) 
                                j_new_rank = i_rank;
                            else{
                                rank_diff = redu_cost / epsilon;
                                j_new_rank = ( rank_diff < linf ) ? i_rank - ( (long)rank_diff + 1 ): 0;
                            }
                            if ( j_rank < j_new_rank ){
                                if ( cc == 1 ){
                                    pj -> rank = j_new_rank;
                                    
                                    if ( j_rank > 0 ){
                                        pb_old = p_buckets + j_rank;
                                        REMOVE_FROM_BUCKET( pj, pb_old );
                                    }
                                    
                                    pb_new = p_buckets + j_new_rank;
                                    INSERT_TO_BUCKET( pj, pb_new );  
                                }else{
                                    df = pa -> resi_cap;
                                    INCREASE_FLOW( pi, pj, pa, df ); 
                                }
                            }
                        }
                    } /* end if opened arc */
                } /* all arcs are scanned */
                
                pi -> price -= dp;
                
            } /* end of while-cycle: the bucket is scanned */
        } /* end of for-cycle: all buckets are scanned */
        
        if ( cc == 0 ) break;
        
    } /* end of main loop */
    
    /* finish: */
    
    /* if refine needed - saturate non-epsilon-optimal arcs */
    
    if ( cc == 0 ){ 
        for(pi=p_nodes;pi!=p_end_node;pi++) {
            pa_stop=(pi+1)->p_suspend_arc;
            for(pa=pi->p_first_arc;pa!=pa_stop;pa++) {
                if ( REDUCED_COST ( pi, pa -> p_head_node, pa ) < -epsilon ){
                    if ( ( df =pa -> resi_cap ) > 0 ){
                        INCREASE_FLOW( pi, pa -> p_head_node, pa, df );
                    }
                }
                
            }
        }
    }
    
    /*neg_cyc();*/
    return ( cc );
    return 0;
}

void MCFF3::computePrices(){
    
}

void MCFF3::priceOut(){
    Node     *pi;                /* current node */
    
    Arc      *pa,                /* current arc from i */
    *pa_stop,           /* first arc from the next node */
    *pb;                /* arc to be exchanged with suspended */
    
    double   n_cut_off,         /* -cut_off */
    redu_cost;                /* reduced cost */
    
    n_cut_off = - cut_off;
    
    for(pi=p_nodes;pi!=p_end_node;pi++){
        pa_stop=(pi+1)->p_suspend_arc;
        for(pa=pi->p_first_arc;pa!=pa_stop;pa++) {
            redu_cost = REDUCED_COST ( pi, pa -> p_head_node, pa );
            
            if (((redu_cost > cut_off) && (CLOSED(pa -> p_sisiter_arc)))
                ||
                ((redu_cost < n_cut_off) && (CLOSED(pa)))
                )
            { /* suspend the arc */
                pb = pi -> p_first_arc;
                pi->p_first_arc++;
                
                EXCHANGE ( pa, pb );
            }
        }
    }
}

int MCFF3::updateEpsilon(){
    if ( epsilon <= 1 ) 
        return 1;
    epsilon = (cost_t) (ceil((double) epsilon / f_scale));
    cut_off = cut_off_factor * epsilon;
    cut_on = cut_off * CUT_OFF_GAP;
    
    return 0;
}

int MCFF3::checkCS(){
    return 0;
}

int MCFF3::checkEpsOpt(){
    return 0;
}

long MCFF3::finishUp(){
    
    Arc   *pa;            /* current arc */
    long  na;            /* corresponding position in capacity array */
    long  flow_cost=0;/* flow cost */
    cost_t cs;          /* actual arc cost */
    long   flow;         /* flow through an arc */
    Node *pi;
    
    /*清除之前的 adj_list*/
    if(this->is_calc_path){
        for(int i=0;i<MAX_NETWORK_NODE_NUM+2;i++){
            adj_list[i].clear();
        }
    }
    
    
    for ( pa = p_arcs, na = 0; pa != p_end_arc ; pa ++, na ++ ){
        cs = pa -> cost / cost_mltiplier;
        /*cap[na]表示arc na的容量，a->resi_cap表示arc a的残余量，流量=容量-残余量*/
        if ( p_cap[na]  > 0 && (flow= p_cap[na]-(pa -> resi_cap)) != 0 ){
            flow_cost +=(long) cs * flow; 
            if(this->is_calc_path){
                /*构建最终有效图的邻接表*/
                uint from,to,cap,cost;
                from=(uint)(pa->p_sisiter_arc->p_head_node-p_nodes);
                to=(uint)(pa->p_head_node-p_nodes);
                cap=(uint)flow;
                cost=(uint)cs;
                addEdge2( from,to,cap,cost);
            }
        }
        pa -> cost = cs;
    }
    
    for(pi=p_nodes;pi!=p_end_node;pi++){
        pi->price = (pi->price / cost_mltiplier);
    }
    
    return flow_cost;
}

void MCFF3::initSolution(){
    
}

long MCFF3::costScaleCore(long scale_fac){
    int cc=0;
    long flow_cost=0;
    initCostScale(scale_fac);
    updateEpsilon();
    
    do{ 
        refine();
        if(is_feasible){
            if(n_ref >= PRICE_OUT_START)
                priceOut();
            if(updateEpsilon()) 
                break;
            while(1){
                if (! priceRefine()) 
                    break;
                
                if (n_ref >= PRICE_OUT_START){
                    if (priceIn()) 
                        break; 
                    if ((cc = updateEpsilon())) 
                        break;
                }
            }
        }else{
            flow_cost=-1;
            return flow_cost;
        }
        
    }while (cc == 0);
    
    flow_cost=finishUp();
    return flow_cost;
}

bool MCFF3::isFeasible(vector<uint> selected_server_id){
    uint sum_cap=0;
    
    for(int i=0;i<selected_server_id.size();i++){
        sum_cap+=graph->getNetNodeFlow(selected_server_id[i]);
    }
    
    if(sum_cap<graph->getTotalNeededFlow()){
        return false;
    }
    return true;
}

long MCFF3::minCostFlowCore(uint super_source,
                            uint super_sink,
                            uint need_flow,
                            vector<Flow>& best_paths){
    best_paths.clear();
    while(need_flow>0){
        priority_queue<upair,vector<upair>,greater<upair> >q;//min heap 
        fill(distance,distance+n,INF);
        distance[super_source]=0;
        
        q.push(upair(0,super_source));
        
        while(!q.empty()){
            upair p=q.top();
            q.pop();
            uint v=p.second;
            if(distance[v]<p.first)
                continue;
            //traverse the adj_list of node v
            for(uint i=0;i<adj_list[v].size();i++){
                Edge2& e=adj_list[v][i];
                if(e.capacity>0 && distance[e.end_node]>distance[v]+e.capacity){
                    distance[e.end_node]=distance[v]+e.capacity;
                    pre_vertex[e.end_node]=v;
                    pre_edg2[e.end_node]=i;
                    q.push(upair(distance[e.end_node],e.end_node));
                }
            }
        }
        
        if(distance[super_sink]==INF)
            return -1;
        
        uint real_flow=need_flow;
        for(uint i=super_sink;i!=super_source;i=pre_vertex[i])		  
            real_flow=min(real_flow,adj_list[pre_vertex[i]][pre_edg2[i]].capacity);
        need_flow-=real_flow;
        
        //create flow and push it into best_paths
        Flow flow;
        for(int x=super_sink;pre_vertex[x]!=super_source;x=pre_vertex[x])
            flow.route.insert(flow.route.begin(),pre_vertex[x]);
        uint consum_id=graph->getConsumNodeId(*(flow.route.end()-1));
        flow.consum_id=consum_id;	
        flow.bandwidth=real_flow;
        best_paths.push_back(flow);
        
        //reduce capacity of edge
        for(int i=super_sink;i!=super_source;i=pre_vertex[i]){
            Edge2 & e=adj_list[pre_vertex[i]][pre_edg2[i]];
            e.capacity-=real_flow;
        }
        
    }
    return 0;
}

long MCFF3::getMinCost(vector<uint> selected_server_id){
    if(!isFeasible(selected_server_id)){
        return -1;
    }
    
    long flow_cost=0;
    long total_cost=-1;
    long f_sc= SCALE_DEFAULT;
    
    this->is_calc_path=false;
    super_edge_list.clear();	
    
    uint super_source=graph->getNetNodeNum();
    uint super_sink=super_source+1;
    
    createSuperEdgeList(selected_server_id,super_source,super_sink);
    initNetworkTopo(super_source,super_sink );
    
    flow_cost=costScaleCore(f_sc);
    
    if(!is_feasible){
        is_feasible=true;
    }
    
    if(flow_cost!=-1){
        total_cost=flow_cost+selected_server_id.size()*(graph->getPerServerCost());
    }
    
    free(p_cap);
    free(p_nodes - min_node_id);
    free(p_arcs);
    free(p_buckets);
    return total_cost;
}

long MCFF3::getMinCostFlow(vector<uint> best_server_id,
                           vector<Flow>& best_paths){
    long flow_cost=0;
    long total_cost=-1;
    long f_sc= SCALE_DEFAULT;
    
    this->is_calc_path=true;
    super_edge_list.clear();	
    
    uint super_source=graph->getNetNodeNum();
    uint super_sink=super_source+1;
    
    createSuperEdgeList(best_server_id,super_source,super_sink);
    initNetworkTopo(super_source,super_sink );
    
    flow_cost=costScaleCore(f_sc);
    
    if(!is_feasible){
        is_feasible=true;
    }
    
    if(flow_cost!=-1){
        total_cost=flow_cost+best_server_id.size()*(graph->getPerServerCost());
        minCostFlowCore(super_source,super_sink,graph->getTotalNeededFlow(),best_paths);
    }
    free(p_cap);
    free(p_nodes - min_node_id);
    free(p_arcs);
    free(p_buckets);
    return total_cost;
}

void MCFF3::printFlowInfo(long total_cost,
                          vector<Flow>& best_paths){
    
    cout<<total_cost<<endl<<endl;
    cout<<best_paths.size()<<endl<<endl;
    
    for(int i=0;i<best_paths.size();i++){
        Flow flow=best_paths[i];
        vector<uint>::iterator iter=flow.route.begin();
        for(;iter!=flow.route.end();iter++){
            std::cout<<*iter<<" ";
        }
        cout<<flow.consum_id<<" "<<flow.bandwidth<<endl;
    }
    cout<<endl;
}
