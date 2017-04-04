#include "m_pd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filter.h"


static t_class *mind_filter_class;


typedef struct _mind_filter{
        t_object x_obj;
        t_outlet *out;
	int size;
        double *x, *y;
	const double *a, *b;
        
} t_mind_filter;


void *mind_filter_new(t_symbol *arg){
	t_mind_filter *x = (t_mind_filter*) pd_new(mind_filter_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	
	if(strcmp(arg->s_name,"delta")==0){
		x->a = a_delta;
		x->b = b_delta;
		x->size = 7;
	}
	else{
		if(strcmp(arg->s_name,"theta")==0){
			x->a = a_theta;
			x->b = b_theta;
			x->size = 9;
		}
		else{
			if(strcmp(arg->s_name,"low_alpha")==0){
				x->a = a_lowalpha;
				x->b = b_lowalpha;
				x->size = 9;
			}
			else{
				if(strcmp(arg->s_name,"high_alpha")==0){
        	                        x->a = a_highalpha;
                	                x->b = b_highalpha;
                        	        x->size = 9;
	                        }
                        	else{
					if(strcmp(arg->s_name,"low_beta")==0){
		                                x->a = a_lowbeta;
                	                	x->b = b_lowbeta;
                        		        x->size = 11;
                        		}
                        		else{
						if(strcmp(arg->s_name,"high_beta")==0){
			                                x->a = a_highbeta;
                        		        	x->b = b_highbeta;
                                			x->size = 11;
                        			}
                        			else{
							if(strcmp(arg->s_name,"low_gamma")==0){
                                				x->a = a_lowgamma;
                                				x->b = b_lowgamma;
                                				x->size = 11;
                        				}
                        				else{
								if(strcmp(arg->s_name,"mid_gamma")==0){
                                					x->a = a_midgamma;
                                					x->b = b_midgamma;
                                					x->size = 11;
                        					}
                        					else{
									post("Couldn't create: EEG Band not found");
                        					}
                        				}
                        			}
                        		}
                        	}
			}
		}
	}
	x->x = malloc(x->size*sizeof(double));
	x->y = malloc(x->size*sizeof(double)); 
	for(int i = 0;i < x->size;i++){
		x->x[i]=0;
		x->y[i]=0;
	}
	
		
	return (void*) x;
}

static void mind_filter_destroy(t_mind_filter *x){
}
static void mind_filter_float(t_mind_filter *x, t_floatarg sample){
 
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

void mind_filter_setup(void){
        mind_filter_class = class_new(gensym("mind_filter"), (t_newmethod) mind_filter_new,(t_method) mind_filter_destroy , sizeof(t_mind_filter),CLASS_DEFAULT, A_DEFSYMBOL, 0);
        class_addfloat(mind_filter_class,mind_filter_float);
}
  
