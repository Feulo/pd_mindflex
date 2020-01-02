#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "filter.h"

static t_class *mind_blink_class;

typedef struct _mind_blink{
        t_object x_obj;
	t_float threshold;
	t_outlet *out;
	double *x,*y ;
	int cooldown;
	const double *a, *b;
} t_mind_blink;


void *mind_blink_new(t_floatarg threshold){
        t_mind_blink *x = (t_mind_blink*) pd_new(mind_blink_class);
        x->threshold = threshold;
	floatinlet_new(&x->x_obj, &x->threshold);
        x->out = outlet_new(&x->x_obj, &s_bang);
	x->a =a_lp;
	x->b =b_lp;
	x->cooldown= 51;
	x->x = malloc(7*sizeof(double));
	x->y = malloc(7*sizeof(double));

	for (int i = 0; i<7;i++){
		x->x[i]=0;
		x->y[i]=0;
	}
	return (void *)x;
}

static void mind_blink_float(t_mind_blink *x, t_floatarg sample){
 	
	double temp=0;
	for(int i =6;i>0;i--){
		x->x[i] = x->x[i-1];
		x->y[i] = x->y[i-1];
		temp+= x->b[i]*x->x[i]-x->a[i]*x->y[i];
	}
	x->x[0] = sample;
	x->y[0] = temp+x->b[0]*x->x[0];
	
	x->cooldown++;
	if(sample> x->threshold && x->x[1] > x->x[2] && x->x[1] > sample && x->cooldown>102){
		outlet_bang(x->out);
		x->cooldown=0;
	}
}

void mind_blink_setup(void){
        mind_blink_class = class_new(gensym("mind_blink"), (t_newmethod) mind_blink_new,0, sizeof(t_mind_blink),CLASS_DEFAULT, A_DEFFLOAT,0);
        class_addfloat(mind_blink_class,mind_blink_float);
}

