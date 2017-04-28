#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
static t_class *mind_attention_class;

typedef struct _mind_attention{
	t_object x_obj;
	t_float threshold;
	t_outlet *out1,*out2;
} t_mind_attention;


void *mind_attention_new(t_floatarg threshold){
	t_mind_attention *x = (t_mind_attention*) pd_new(mind_attention_class);
	if(threshold ==0) x->threshold = 50;
	else x->threshold = threshold;
	x->out1 = outlet_new(&x->x_obj, &s_bang);
	x->out2 = outlet_new(&x->x_obj,	&s_bang);
	floatinlet_new(&x->x_obj, &x->threshold);
	return (void *)x;
}

void mind_attention_float(t_mind_attention *x, t_floatarg value){
	if(value>x->threshold) outlet_bang(x->out2);
	else outlet_bang(x->out1);


}
void mind_attention_setup(void){
	mind_attention_class = class_new(gensym("mind_attention"), (t_newmethod) mind_attention_new,0, sizeof(t_mind_attention),CLASS_DEFAULT, A_DEFFLOAT, 0);
	class_addfloat(mind_attention_class, (t_method) mind_attention_float);
}
