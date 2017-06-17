#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static t_class *mind_sequence_class;

typedef struct _mind_sequence{
	t_object x_obj;
	FILE *fp;
	char* arq; 
} t_mind_sequence;


void *mind_sequence_new(t_symbol *arg){
	t_mind_sequence *x = (t_mind_sequence*) pd_new(mind_sequence_class);
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
	x->fp = NULL;	
	return (void *)x;
}

static void mind_sequence_seq(t_mind_sequence *x, t_symbol *arg){
	x->fp = fopen(x->arq,"w");
	fprintf(x->fp,"%s\n",arg->s_name);
	fclose(x->fp);
}

static void mind_sequence_set(t_mind_sequence* x, t_symbol *arg){
	free(x->arq); 
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
}

void mind_sequence_setup(void){
	mind_sequence_class = class_new(gensym("mind_sequence"), (t_newmethod) mind_sequence_new,0 , sizeof(t_mind_sequence),CLASS_DEFAULT, A_DEFSYMBOL, 0);
	class_addmethod(mind_sequence_class,(t_method) mind_sequence_seq,gensym("seq"),A_DEFSYMBOL,0);
	class_addmethod(mind_sequence_class,(t_method) mind_sequence_set,gensym("file"),A_DEFSYMBOL,0);
}
