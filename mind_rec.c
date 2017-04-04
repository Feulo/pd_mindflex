#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
static t_class *mind_rec_class;

typedef struct _mind_rec{
	t_object x_obj;
	FILE *fp;
	t_int recording;
	char* arq; 
} t_mind_rec;


void *mind_rec_new(t_symbol *arg){
	t_mind_rec *x = (t_mind_rec*) pd_new(mind_rec_class);
	x->recording=0;
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
	x->fp = NULL;	
	return (void *)x;
}

static void mind_rec_destroy(t_mind_rec *x){
	if(x->fp!=NULL)
		fclose(x->fp);
	free(x->arq);	
}
static void mind_rec_float(t_mind_rec *x, t_floatarg sample){
	if(x->recording==1){
		unsigned char* c = (unsigned char*) & sample;
		for(int i =0;i< sizeof(t_floatarg);i++)
			fputc(c[i],x->fp);
	}	
}


static void mind_rec_stop(t_mind_rec *x){
	x->recording = 0;
	if(x->fp!=NULL){
		fclose(x->fp);
		x->fp=NULL;
	}
}

FILE * cria(char* arq){
	FILE* fp;
	char* temp = malloc((strlen(arq)+1)*sizeof(char));
	strcpy(temp,arq);
	strcat(temp,".RAW");
	if(access(temp,F_OK) != -1){

		free(temp);
		return cria(strcat(arq,"(1)"));
	}
	else{

		fp = fopen(temp,"w");
		free(temp);
		return fp;
	}
}
static void mind_rec_rec(t_mind_rec *x){
FILE* fp = cria(x->arq);
x->fp = fp;
x->recording =1;
}

static void mind_rec_set(t_mind_rec* x, t_symbol *arg){
	free(x->arq); 
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
}

void mind_rec_setup(void){
	mind_rec_class = class_new(gensym("mind_rec"), (t_newmethod) mind_rec_new,(t_method) mind_rec_destroy , sizeof(t_mind_rec),CLASS_DEFAULT, A_DEFSYMBOL, 0);
	class_addfloat(mind_rec_class,mind_rec_float);
	class_addmethod(mind_rec_class, (t_method) mind_rec_rec, gensym("start"),0);
	class_addmethod(mind_rec_class,(t_method) mind_rec_stop,gensym("stop"),0);
	class_addmethod(mind_rec_class,(t_method) mind_rec_set,gensym("file"),A_DEFSYMBOL,0);
}
