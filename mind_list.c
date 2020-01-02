#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
static t_class *mind_list_class;

typedef struct _mind_list{
	t_object x_obj;
	FILE *fp;
	t_int recording;
	char* arq; 
} t_mind_list;


void *mind_list_new(t_symbol *arg){
	t_mind_list *x = (t_mind_list*) pd_new(mind_list_class);
	x->recording=0;
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
	x->fp = NULL;	
	return (void *)x;
}

static void mind_list_destroy(t_mind_list *x){
	if(x->fp!=NULL)
		fclose(x->fp);
	free(x->arq);	
}
static void mind_list_float(t_mind_list *x, t_floatarg sample){
	if(x->recording==1){
		fprintf(x->fp,"%f\n",sample);
	}	
}


static void mind_list_stop(t_mind_list *x){
	x->recording = 0;
	if(x->fp!=NULL){
		fclose(x->fp);
		x->fp=NULL;
	}
}


static void mind_list_rec(t_mind_list *x){
x->fp = fopen(x->arq,"w");
x->recording =1;
}

static void mind_list_set(t_mind_list* x, t_symbol *arg){
	free(x->arq); 
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
}

void mind_list_setup(void){
	mind_list_class = class_new(gensym("mind_list"), (t_newmethod) mind_list_new,(t_method) mind_list_destroy , sizeof(t_mind_list),CLASS_DEFAULT, A_DEFSYMBOL, 0);
	class_addfloat(mind_list_class,mind_list_float);
	class_addmethod(mind_list_class, (t_method) mind_list_rec, gensym("start"),0);
	class_addmethod(mind_list_class,(t_method) mind_list_stop,gensym("stop"),0);
	class_addmethod(mind_list_class,(t_method) mind_list_set,gensym("file"),A_DEFSYMBOL,0);
}
