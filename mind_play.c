#include "m_pd.h"
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PI 3.14159265358979323846
#define TABLE_SIZE 512 

int nano_sleep(int nsec) {
        struct timespec ts1, ts2;
        ts1.tv_sec = 0;
        ts1.tv_nsec = nsec;
        if (nanosleep(&ts1 , &ts2) < 0) {
                return -1;
        }
        return 0;
}

static t_class *mind_play_class;

typedef struct _mind_play{
	t_object x_obj;
	t_outlet *out;
	FILE *fp;
	pthread_t thread;
	char* arq;
} t_mind_play;


void *mind_play_new(t_symbol *arg){
	t_mind_play *x = (t_mind_play*) pd_new(mind_play_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	x->arq = malloc((strlen(arg->s_name)+1)*sizeof(char));
	strcpy(x->arq,arg->s_name);
	return (void *)x;
}

static void mind_play_destroy(t_mind_play *x){
	pthread_cancel(x->thread);
	free(x->arq);
	if(x->fp!=NULL)
		fclose(x->fp);
}
void *mind_play_thread (void * arg){
	t_mind_play *x = (t_mind_play*) arg;
	char c[sizeof(float)];
	float* temp;
	while(1){
		for(int i=0;i<sizeof(float);i++){
			c[i] = fgetc(x->fp);
			if(c[i]==EOF)
				return 0;	
		}
		temp = (float*)c;
		outlet_float(x->out,*temp);
		nano_sleep(1.0/512.0 * 1e9);
	}
	return 0;
}

static void mind_play_bang (t_mind_play *x){
	x->fp = fopen(x->arq,"r");
	pthread_create(&x->thread, NULL, mind_play_thread, x);
}

void mind_play_setup(void){
	mind_play_class = class_new(gensym("mind_play"), (t_newmethod) mind_play_new,(t_method) mind_play_destroy , sizeof(t_mind_play),CLASS_DEFAULT, A_DEFSYMBOL, 0);
	class_addbang(mind_play_class, mind_play_bang);	
}
