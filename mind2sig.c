#include <samplerate.h>
#include <pthread.h>
#include "m_pd.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.14159265358979323846
#define TABLE_SIZE 2048
#define B_OUT_SIZE 12000
#define B_IN_SIZE 4 

static t_class *mind2sig_class;

typedef struct _mind2sig{
	t_object x_obj;
	t_outlet *out;
	
	float b_out[B_OUT_SIZE];
	int   n_in;
	int   n_out_r, n_out_w;
	float b_in[B_IN_SIZE];
		
	SRC_DATA src_data;
	SRC_STATE *src_state;
	
	t_int buf_empty;
	pthread_t con;
	pthread_mutex_t the_mutex;
	pthread_cond_t condc,condp;

} t_mind2sig;


static t_int *mind2sig_perform(t_int *w){
	
	t_mind2sig *x = (t_mind2sig*) (w[1]);
	t_float *out = (t_float*) (w[2]);
	int n = (int) (w[3]);
	
	while(n--)*out++= x->b_out[x->n_out_r++ % B_OUT_SIZE];
	return (w+4);
}

static void mind2sig_dsp(t_mind2sig *x, t_signal **sp){
	x->n_out_r = (x->n_out_w - 950)% B_OUT_SIZE;
	dsp_add(mind2sig_perform, 3, x,sp[0]->s_vec,sp[0]->s_n);
}

static void mind2sig_float(t_mind2sig *x, t_floatarg f){

	x->b_in[x->n_in++] = f;

	if(x->n_in >= B_IN_SIZE){
	//mutex lock
                pthread_mutex_lock(&x->the_mutex);
        // se o buffer nao tive vazio espera
                while (!x->buf_empty) {
                        pthread_cond_wait(&x->condp, &x->the_mutex);
                }
        //copia do local_buf pro buf
                //for(int i=0;i<x->buf_size;i++)
                 //       x->buf[i]=x->local_buf[i];
        //seta buffer nao vazio
                x->buf_empty = 0;
        //sinaliza pro consumidor
                pthread_cond_signal(&x->condc);

        //destrava o buffer
                pthread_mutex_unlock(&x->the_mutex);
        //count=0
                x->n_in = 0;

        }	
}
void *mind2sig_thread(void *arg){
	t_mind2sig *x = (t_mind2sig*) arg;
	t_float in[4];
	int error;
	x->src_data.data_in = in;
	while(1){
        //trancar o mutex
                pthread_mutex_lock(&x->the_mutex);
        //se o buffer vazio entao espera
                while (x->buf_empty) {
                        pthread_cond_wait(&x->condc, &x->the_mutex);
                }
        //faz o role
                for(int i=0;i<4;i++){
			in[i]=x->b_in[i];
			x->b_in[i] = 0;
		}
        // seta buffer empty
                x->buf_empty=1;
        //acorda o produtor
                pthread_cond_signal(&x->condp);
        //solta o buffer
                pthread_mutex_unlock(&x->the_mutex);
	
		x->src_data.data_out =x->b_out+x->n_out_w;
		x->src_data.output_frames = 475;

		if ((error = src_process (x->src_state,& x->src_data)))
                       post("\nError : %s\n", src_strerror (error)) ;
                if(x->src_data.input_frames_used<4) post("uso só: %d",x->src_data.input_frames_used);
		x->n_out_w =(x->n_out_w+ x->src_data.output_frames_gen)% B_OUT_SIZE;	        

	}
}

void *mind2sig_new(){
	int erro =0;
	t_mind2sig *x = (t_mind2sig*) pd_new(mind2sig_class);
	x->out = outlet_new(&x->x_obj, &s_signal);
	
	for(int i=0;i<B_OUT_SIZE;i++)x->b_out[i]=0;
	for(int i=0;i<B_IN_SIZE;i++) x->b_in[i]=0;
	x->n_in=0;
	x->n_out_r=0;
	x->n_out_w = 0;
	if((x->src_state = src_new(SRC_SINC_BEST_QUALITY,1,&erro))== NULL){
		post("erro: %s",src_strerror(erro));
	}
	x->src_data.end_of_input = 0 ; /* Set this later. */

        /* Start with zero to force load in while loop. */
        x->src_data.input_frames = 4 ;
        x->src_data.src_ratio = sys_getsr()/512.0 ;
        x->src_data.output_frames = 94 ;

	x->buf_empty =1;
	pthread_mutex_init(&x->the_mutex,NULL);
	pthread_cond_init(&x->condc,NULL);
	pthread_cond_init(&x->condp,NULL);
	pthread_create(&x->con,NULL,mind2sig_thread,x);
	return (void *)x;
}

void mind2sig_destroy(t_mind2sig *x){

	pthread_cancel(x->con);
	pthread_cond_destroy(&x->condc);
	pthread_cond_destroy(&x->condp);
	pthread_mutex_destroy(&x->the_mutex);
}

void mind2sig_setup(void){
	mind2sig_class = class_new(gensym("mind2sig"), (t_newmethod) mind2sig_new, (t_method) mind2sig_destroy, sizeof(t_mind2sig),CLASS_DEFAULT,0);
	class_addfloat(mind2sig_class,mind2sig_float);
	class_addmethod(mind2sig_class, (t_method) mind2sig_dsp, gensym("dsp"), A_CANT,0);

}
