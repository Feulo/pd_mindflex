#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static t_class *mind_blink_class;

typedef struct _mind_blink{
        t_object x_obj;
	t_float threshold;
	t_outlet *out;
} t_mind_blink;


void *mind_blink_new(t_floatarg threshold){
        t_mind_blink *x = (t_mind_blink*) pd_new(mind_blink_class);
        x->threshold = threshold;
	floatinlet_new(&x->x_obj, &x->threshold);
        x->out = outlet_new(&x->x_obj, &s_bang);
	return (void *)x;
}

static void mind_blink_float(t_mind_blink *x, t_floatarg sample){
 	
	if(sample> x->threshold)  outlet_bang(x->out);     
}

void mind_blink_setup(void){
        mind_blink_class = class_new(gensym("mind_blink"), (t_newmethod) mind_blink_new,0, sizeof(t_mind_blink),CLASS_DEFAULT, A_DEFFLOAT,0);
        class_addfloat(mind_blink_class,mind_blink_float);
}

