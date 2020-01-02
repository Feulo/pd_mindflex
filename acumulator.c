#include "m_pd.h"  
 
static t_class *acumulator_class;  
 
typedef struct _acumulator {  
  t_object  x_obj;  
  t_int i_count;  
  t_outlet *f_out;  
} t_acumulator;  
 
void acumulator_float(t_acumulator *x,t_float sample)  
{  
  x->i_count+=sample;  
  outlet_float(x->f_out, x->i_count);  
}  
 
void acumulator_reset(t_acumulator *x)  
{  
  x->i_count = 0;  
}  
 
void *acumulator_new()  
{  
  t_acumulator *x = (t_acumulator *)pd_new(acumulator_class);  
   x->i_count=0;  
   x->f_out = outlet_new(&x->x_obj, &s_float);   
  return (void *)x;  
}  
 
void acumulator_setup(void) {  
  acumulator_class = class_new(gensym("acumulator"),  
        (t_newmethod)acumulator_new,  
        0, sizeof(t_acumulator),  
        CLASS_DEFAULT, 0);  
 
  class_addfloat  (acumulator_class, acumulator_float);  
  class_addmethod(acumulator_class, (t_method)acumulator_reset, gensym("reset"), 0);    
} 
