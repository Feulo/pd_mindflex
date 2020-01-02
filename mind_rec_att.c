#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static t_class *mind_rec_att_class;

typedef struct _mind_rec_att{
	t_object x_obj;
	FILE *fp;
	t_int recording;
	char* arq; 
} t_mind_rec_att;


void *mind_rec_att_new(t_symbol *arg){
	t_mind_rec_att *x = (t_mind_rec_att*) pd_new(mind_rec_att_class);
	x->recording=0;
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
	x->fp = NULL;	
	return (void *)x;
}

static void mind_rec_att_destroy(t_mind_rec_att *x){
	if(x->fp!=NULL)
		fclose(x->fp);
	free(x->arq);	
}
static void mind_rec_att_float(t_mind_rec_att *x, t_floatarg sample){
	if(x->recording==1){
		fprintf(x->fp,"%f\n",sample);
	}	
}


static void mind_rec_att_stop(t_mind_rec_att *x){
	x->recording = 0;
	if(x->fp!=NULL){
		fclose(x->fp);
		x->fp=NULL;
	}
}


static void mind_rec_att_rec_att(t_mind_rec_att *x){

	x->fp = fopen(x->arq,"w");
	x->recording =1;
}

static void mind_rec_att_set(t_mind_rec_att* x, t_symbol *arg){
	free(x->arq); 
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
}

void mind_rec_att_setup(void){
	mind_rec_att_class = class_new(gensym("mind_rec_att"), (t_newmethod) mind_rec_att_new,(t_method) mind_rec_att_destroy , sizeof(t_mind_rec_att),CLASS_DEFAULT, A_DEFSYMBOL, 0);
	class_addfloat(mind_rec_att_class,mind_rec_att_float);
	class_addmethod(mind_rec_att_class, (t_method) mind_rec_att_rec_att, gensym("start"),0);
	class_addmethod(mind_rec_att_class,(t_method) mind_rec_att_stop,gensym("stop"),0);
	class_addmethod(mind_rec_att_class,(t_method) mind_rec_att_set,gensym("file"),A_DEFSYMBOL,0);
}
