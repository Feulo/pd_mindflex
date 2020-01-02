#include "m_pd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filter.h"


static t_class *mind_lp_class;


typedef struct _mind_lp{
        t_object x_obj;
        t_outlet *out;
	int size;
        double *x, *y;
	const double *a, *b;
        
} t_mind_lp;


void *mind_lp_new(t_symbol *arg){
	t_mind_lp *x = (t_mind_lp*) pd_new(mind_lp_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	

		x->a = a_lp;
		x->b = b_lp;
		x->size = 7;
		
	
	x->x = malloc(x->size*sizeof(double));
	x->y = malloc(x->size*sizeof(double)); 
	for(int i = 0;i < x->size;i++){
		x->x[i]=0;
		x->y[i]=0;
	}
	
		
	return (void*) x;
}

static void mind_lp_destroy(t_mind_lp *x){
}
static void mind_lp_float(t_mind_lp *x, t_floatarg sample){
 
	double temp=0;
	for(int i=x->size-1;i>0;i--){
		x->x[i]=x->x[i-1];
		x->y[i]=x->y[i-1];
		temp+= x->b[i]*x->x[i]-x->a[i]*x->y[i];
	}
	x->x[0] = sample;
	x->y[0] = temp+x->b[0]*x->x[0];
	outlet_float(x->out,x->y[0]);      
}

void mind_lp_setup(void){
        mind_lp_class = class_new(gensym("mind_lp"), (t_newmethod) mind_lp_new,(t_method) mind_lp_destroy , sizeof(t_mind_lp),CLASS_DEFAULT, A_DEFSYMBOL, 0);
        class_addfloat(mind_lp_class,mind_lp_float);
}
  
