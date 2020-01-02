#include "m_pd.h"  
 
static t_class *modcounter_class;  
 
typedef struct _modcounter {  
  t_object  x_obj;  
  t_int i_count;
  t_int mod;  
} t_modcounter;  
 
void modcounter_bang(t_modcounter *x)  
{  
  t_float f=(x->i_count+1) % x->mod;  
  x->i_count=f;  
  outlet_float(x->x_obj.ob_outlet, f);  
}  
void modcounter_reset(t_modcounter *x){
	x->i_count =0;	
  	outlet_float(x->x_obj.ob_outlet, 0);  
}
 
void *modcounter_new(t_floatarg f)  
{  
  t_modcounter *x = (t_modcounter *)pd_new(modcounter_class);  
 
  x->i_count=0;
  x->mod = f;  
  outlet_new(&x->x_obj, &s_float);  
 
  return (void *)x;  
}  
 
void modcounter_setup(void) {  
  modcounter_class = class_new(gensym("modcounter"),  
        (t_newmethod)modcounter_new,  
        0, sizeof(t_modcounter),  
        CLASS_DEFAULT,  
        A_DEFFLOAT, 0);  
 
  class_addbang(modcounter_class, modcounter_bang);
  class_addmethod(modcounter_class,(t_method) modcounter_reset, gensym("reset"),0);  
}
