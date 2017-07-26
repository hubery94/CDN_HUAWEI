

#ifndef __MCFF3_MACRO__
#define __MCFF3_MACRO__

#include "mcff3.h"

#define MAX_64 (0x7FFFFFFFFFFFFFFFLL) 
#define MAX_32 (0x7FFFFFFF)
#define PRICE_MAX MAX_64//
#define N_NODE( i ) ( ( (i) == NULL ) ? -1 : ( (i) - ndp + nmin ) )
#define N_ARC( a ) ( ( (a) == NULL )? -1 : (a) - arp )


#define UNFEASIBLE          1//
#define ALLOCATION_FAULT    2
#define PRICE_OFL           3

/* parameters */
#define UPDT_FREQ      0.4
#define UPDT_FREQ_S    30

#define SCALE_DEFAULT  12.0

/* PRICE_OUT_START may not be less than 1 */
#define PRICE_OUT_START 1

#define CUT_OFF_POWER    0.44//
#define CUT_OFF_COEF     1.5//
#define CUT_OFF_POWER2   0.75
#define CUT_OFF_COEF2    1
#define CUT_OFF_GAP      0.8
#define CUT_OFF_MIN      12//
#define CUT_OFF_INCREASE 4

/*
#define TIME_FOR_PRICE_IN    5
*/
#define TIME_FOR_PRICE_IN1    2
#define TIME_FOR_PRICE_IN2    4
#define TIME_FOR_PRICE_IN3    6


#define MAX_CYCLES_CANCELLED 0
#define START_CYCLE_CANCEL   100

/************************************************ shared macros *******/

#define MAX(x,y) (((x)>(y))?x:y)//
#define MIN(x,y) (((x)<(y))?x:y)//


#define OPEN(a)   (a -> resi_cap > 0 )
#define CLOSED(a)   (a -> resi_cap <= 0 )
#define REDUCED_COST(i, j, a ) ( (i->price) + (a->cost) - (j->price) )
#define FEASIBLE(i,j, a )     ( (i->price) + (a->cost) < (j->price) )
#define ADMISSIBLE( i, j, a )   ( OPEN(a) && FEASIBLE( i, j, a ) )



#define INCREASE_FLOW( i, j, a, df )\
{\
	(i) -> excess            -= df;\
	(j) -> excess            += df;\
	(a)            -> resi_cap  -= df;\
	((a) -> p_sisiter_arc) -> resi_cap  += df;\
}\



/*---------------------------------- macros for excess queue */
#define RESET_EXCESS_Q \
{\
	for ( ; p_queue_first != NULL; p_queue_first = p_queue_last )\
	{\
	p_queue_last            = p_queue_first -> p_queue_next;\
	p_queue_first -> p_queue_next = p_end_node;\
	 }\
}

#define OUT_OF_EXCESS_Q( i )  ( i -> p_queue_next == p_end_node )

#define EMPTY_EXCESS_Q    ( p_queue_first == NULL )
#define NONEMPTY_EXCESS_Q ( p_queue_first != NULL )

#define INSERT_TO_EXCESS_Q( i )\
{\
	if ( NONEMPTY_EXCESS_Q )\
	p_queue_last -> p_queue_next = i;\
   else\
   p_queue_first  = i;\
   \
   i -> p_queue_next = NULL;\
   p_queue_last   = i;\
}

#define INSERT_TO_FRONT_EXCESS_Q( i )\
{\
	if ( EMPTY_EXCESS_Q )\
	p_queue_last = i;\
	\
	i -> p_queue_next = p_queue_first;\
	p_queue_first  = i;\
}

#define REMOVE_FROM_EXCESS_Q( i )\
{\
	i = p_queue_first;\
	p_queue_first  = i -> p_queue_next;\
	i -> p_queue_next = p_end_node;\
}

/*---------------------------------- excess queue as a stack */

#define EMPTY_STACKQ      EMPTY_EXCESS_Q
#define NONEMPTY_STACKQ   NONEMPTY_EXCESS_Q

#define RESET_STACKQ  RESET_EXCESS_Q

#define STACKQ_PUSH( i )\
{\
	i -> p_queue_next = p_queue_first;\
	p_queue_first  = i;\
}

#define STACKQ_POP( i ) REMOVE_FROM_EXCESS_Q( i )


/*------------------------------------ macros for buckets */
Node	dnd, *dnode;
#define RESET_BUCKET( b )  ( b -> p_first_node ) = dnode;

#define INSERT_TO_BUCKET( i, b )\
{\
	i -> p_buckets_next                  = ( b -> p_first_node );\
	( b -> p_first_node ) -> p_buckets_prev   = i;\
	( b -> p_first_node )             = i;\
}

#define NONEMPTY_BUCKET( b ) ( ( b -> p_first_node ) != dnode )

#define GET_FROM_BUCKET( i, b )\
{\
	i    = ( b -> p_first_node );\
	( b -> p_first_node ) = i -> p_buckets_next;\
}

#define REMOVE_FROM_BUCKET( i, b )\
{\
	if ( i == ( b -> p_first_node ) )\
	( b -> p_first_node ) = i -> p_buckets_next;\
  else\
{\
	( i -> p_buckets_prev ) -> p_buckets_next = i -> p_buckets_next;\
	( i -> p_buckets_next ) -> p_buckets_prev = i -> p_buckets_prev;\
}\
}






/*------------------------------------------- misc macros */
#define UPDATE_CUT_OFF \
{\
	if (n_bad_pricein + n_bad_relabel == 0) \
{\
	cut_off_factor = CUT_OFF_COEF2 * pow ( (double)n, CUT_OFF_POWER2 );\
	cut_off_factor = MAX ( cut_off_factor, CUT_OFF_MIN );\
	cut_off        = cut_off_factor * epsilon;\
	cut_on         = cut_off * CUT_OFF_GAP;\
}\
	 else\
{\
	cut_off_factor *= CUT_OFF_INCREASE;\
	cut_off        = cut_off_factor * epsilon;\
	cut_on         = cut_off * CUT_OFF_GAP;\
}\
}

#define TIME_FOR_UPDATE \
	( n_rel > n * UPDT_FREQ + n_src * UPDT_FREQ_S )


#define WHITE 0
#define GREY  1
#define BLACK 2

Arc     *sa, *sb;
long    d_cap;

#define EXCHANGE( a, b )\
{\
	if ( a != b )\
{\
	sa = a -> p_sisiter_arc;\
	sb = b -> p_sisiter_arc;\
	\
	d_arc.resi_cap = a -> resi_cap;\
	d_arc.cost  = a -> cost;\
	d_arc.p_head_node  = a -> p_head_node;\
	\
	a -> resi_cap  = b -> resi_cap;\
	a -> cost   = b -> cost;\
	a -> p_head_node   = b -> p_head_node;\
	\
	b -> resi_cap  = d_arc.resi_cap;\
	b -> cost   = d_arc.cost;\
	b -> p_head_node   = d_arc.p_head_node;\
	\
	if ( a != sb )\
{\
	b -> p_sisiter_arc = sa;\
	a -> p_sisiter_arc = sb;\
	sa -> p_sisiter_arc = b;\
	sb -> p_sisiter_arc = a;\
}\
	\
	d_cap       = p_cap[a-p_arcs];\
	p_cap[a-p_arcs] = p_cap[b-p_arcs];\
	p_cap[b-p_arcs] = d_cap;\
}\
}

#define SUSPENDED( i, a ) ( a < i -> p_first_arc ) 
#endif 