#include "m_pd.h"
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


static t_class *mind_energy_class;

typedef struct _mind_energy{
	t_object x_obj;
	t_outlet *out;
	t_outlet *img;

	t_int count;
	t_int buf_size;
	t_float *local_buf,*buf;

	
	
	t_int buffer_empty;
	pthread_t con;
	pthread_mutex_t the_mutex;
	pthread_cond_t condc, condp;	
} t_mind_energy;


static void mind_energy_float(t_mind_energy *x, t_float f){
	x->local_buf[x->count++]= f;
	if(x->count >= x->buf_size){
	//mutex lock
		pthread_mutex_lock(&x->the_mutex);
	// se o buffer nao tive vazio espera
		while (!x->buffer_empty) {
                	pthread_cond_wait(&x->condp, &x->the_mutex);
                }
	//copia do local_buf pro buf
		for(int i=0;i<x->buf_size;i++)
			x->buf[i]=x->local_buf[i];
	//seta buffer nao vazio
		x->buffer_empty = 0;
	//sinaliza pro consumidor
		pthread_cond_signal(&x->condc);

	//destrava o buffer
		pthread_mutex_unlock(&x->the_mutex);
	//count=0
		x->count = 0;
	}
}


void *mind_energy_thread(void *arg){
	t_mind_energy *x = (t_mind_energy *) arg;
	t_float *real = malloc(x->buf_size*sizeof(t_float));
	t_float *imag = malloc(x->buf_size*sizeof(t_float));
	t_float temp,c1,c2,u1,u2,t1,t2,z;
	int j,k,h,m,l1,l2,i1;
	m= (int)log2(x->buf_size);
	while(1){
	//trancar o mutex
		pthread_mutex_lock(&x->the_mutex);
	//se o buffer vazio entao espera
		while (x->buffer_empty) {
                        pthread_cond_wait(&x->condc, &x->the_mutex);
                }
	//faz o role
		for(int i=0;i<x->buf_size;i++){
			real[i] = x->buf[i];
			imag[i] = 0;
		}
	// seta buffer empty
		x->buffer_empty=1;
	//acorda o produtor
		pthread_cond_signal(&x->condp);
	//solta o buffer
		pthread_mutex_unlock(&x->the_mutex);
		//bit reversal
		j = 0;
		h = x->buf_size/2;
   		for (int i=0;i<x->buf_size-1;i++) {
      			if (i < j) {
         			 temp  = real[i];
         		         real[i] = real[j];
         		         real[j] = temp;
         		}
      			k = h;
      			while (k <= j) {
         			j -= k;
         			k >>= 1;
      			}
     		 	j += k;
   		}
		//FFT
		c1 = -1.0; 
   		c2 = 0.0;
   		l2 = 1;
   		for (int l=0;l<m;l++) {
      			l1 = l2;
      			l2 <<= 1;
      			u1 = 1.0; 
      			u2 = 0.0;
      			for (j=0;j<l1;j++) {
         			for (int i=j;i<x->buf_size;i+=l2) {
            				i1 = i + l1;
            				t1 = u1 * real[i1]-u2*imag[i1];
            				t2 = u1 * imag[i1]+u2*real[i1];
            				real[i1] = real[i] - t1; 
            				imag[i1] = imag[i] - t2;
            				real[i] += t1;
            				imag[i] += t2;
         			}
         			z =  u1 * c1 - u2 * c2;
         			u2 = u1 * c2 + u2 * c1;
         			u1 = z;
      			}
      			c2 = -1.0*sqrt((1.0 - c1) / 2.0); 		
      			c1 = sqrt((1.0 + c1) / 2.0);
   		}
	
		for(int i = 0;i<x->buf_size;i++){
			outlet_float(x->img,imag[i]);	
			outlet_float(x->out,real[i]);
		}
	}
	return NULL;
}

void *mind_energy_new(t_floatarg f){
	 
	t_mind_energy *x = (t_mind_energy*) pd_new(mind_energy_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	x->img = outlet_new(&x->x_obj, &s_float);
	x->count = 0;
	x->buf_size = (t_int) f;
	x->local_buf = malloc(x->buf_size*sizeof(t_float));
	x->buf = malloc(x->buf_size*sizeof(t_float));
	for(int i=0;i<x->buf_size;i++){
		x->local_buf[i]=0;
		x->buf[i]=0;
	}
	x->buffer_empty=1;
	pthread_mutex_init(&x->the_mutex, NULL);
        pthread_cond_init(&x->condc, NULL);
        pthread_cond_init(&x->condp, NULL);	
	pthread_create(&x->con,NULL,mind_energy_thread,x);
	return (void *)x;
}

void mind_energy_destroy(t_mind_energy *x){
	pthread_cancel(x->con);
	pthread_cond_destroy(&x->condc);
	pthread_cond_destroy(&x->condp);
	pthread_mutex_destroy(&x->the_mutex);
	free(x->local_buf);
	free(x->buf);
}
void mind_energy_setup(void){
	mind_energy_class = class_new(gensym("mind_energy"), (t_newmethod) mind_energy_new,(t_method) mind_energy_destroy, sizeof(t_mind_energy),CLASS_DEFAULT,A_DEFFLOAT,0);
	class_addfloat(mind_energy_class,mind_energy_float);
}
