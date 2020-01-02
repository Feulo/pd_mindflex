#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static t_class *mind_energy_class;

typedef struct _mind_energy{
        t_object x_obj;
	t_outlet *out;
	t_float sum;
} t_mind_energy;


void *mind_energy_new(t_floatarg threshold){
        t_mind_energy *x = (t_mind_energy*) pd_new(mind_energy_class);
        x->out = outlet_new(&x->x_obj, &s_float);
	x->sum=0;
	return (void *)x;
}

static void mind_energy_float(t_mind_energy *x, t_floatarg sample){
 	x->sum+=sample*sample;	
}

static void mind_energy_bang(t_mind_energy *x){
		outlet_float(x->out,x->sum);
		x->sum =0;
}
void mind_energy_setup(void){
        mind_energy_class = class_new(gensym("mind_energy"), (t_newmethod) mind_energy_new,0, sizeof(t_mind_energy),CLASS_DEFAULT,0);
        class_addfloat(mind_energy_class,mind_energy_float);
	class_addbang(mind_energy_class, mind_energy_bang);
}

