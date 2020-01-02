#include "m_pd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



static t_class *mind_max_class;


typedef struct _mind_max{
        t_object x_obj;
        t_outlet *out;
	t_float max;
              
} t_mind_max;


void *mind_max_new(){
	t_mind_max *x = (t_mind_max*) pd_new(mind_max_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	x->max = 0;	
	return (void*) x;
}

static void mind_max_destroy(t_mind_max *x){
}
static void mind_max_float(t_mind_max *x, t_floatarg sample){
	if(sample>x->max) x->max=sample;
	outlet_float(x->out,x->max);      
}

void mind_max_setup(void){
        mind_max_class = class_new(gensym("mind_max"), (t_newmethod) mind_max_new,(t_method) mind_max_destroy , sizeof(t_mind_max),CLASS_DEFAULT, A_DEFSYMBOL, 0);
        class_addfloat(mind_max_class,mind_max_float);
}
  
