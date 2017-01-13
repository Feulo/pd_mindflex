#include "m_pd.h"
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PI 3.14159265358979323846
#define TABLE_SIZE 512 

typedef struct t_data{
    float * sine_table;
    float index;
}sine_data;

int nano_sleep(int nsec) {
        struct timespec ts1, ts2;
        ts1.tv_sec = 0;
        ts1.tv_nsec = nsec;
        if (nanosleep(&ts1 , &ts2) < 0) {
                return -1;
        }
        return 0;
}

float *  create_sine_table(float * table, float x_f){
    table = malloc(TABLE_SIZE * sizeof(float));
    int i;
//	post("pintao");
    for(i=0; i<TABLE_SIZE; i++){
        table[i] = sin(2.0*PI*i/TABLE_SIZE);
//	post("%d   %f",i,table[i]);
    }
        return table;
}

float get_interpolated_freq(sine_data * data, float frequency, float sr){

        int my_floor = floor(data->index);
        float y = data->index - my_floor;

        // Definicao de indexs circulares
        int index1= (my_floor-1 >= 0)?my_floor - 1:TABLE_SIZE + (my_floor-1);
        int index2= my_floor;
        int index3= (my_floor+1 < TABLE_SIZE)?my_floor + 1:my_floor + 1 - TABLE_SIZE;
        int index4= (my_floor+2 < TABLE_SIZE)?my_floor + 2:my_floor + 2 - TABLE_SIZE;
//      float v_interpolado = table[my_floor]; // SEM INTERPOLACAO
//      float v_interpolado = ((1.0 - (y)) * table[index2] + (y) * table[index3]); // INTERPOLACAO LINEAR

        float v_interpolado =    - ((y    ) * (y - 1) * (y - 2) * data->sine_table[index1])/6    //INTERPOLACAO CUBICA
                                   + ((y + 1) * (y - 1) * (y - 2) * data->sine_table[index2])/2
                                   - ((y + 1) * (y    ) * (y - 2) * data->sine_table[index3])/2
                                   + ((y + 1) * (y    ) * (y - 1) * data->sine_table[index4])/6;

        // proximo index a ser lido para esta frequencia
        data->index += TABLE_SIZE * frequency / sr; // Tamanho da tabela * frequencia / Sample rate
        if (data->index >= TABLE_SIZE) //Truncamento pelo tamanho da tabela seno
                data->index -= TABLE_SIZE;

        return v_interpolado;
}

static t_class *mind_sin_class;

typedef struct _mind_sin{
	t_object x_obj;
	t_outlet *out;
	t_float  x_f;
	pthread_t thread;
	//float *tab;
	sine_data *data;
} t_mind_sin;


void *mind_sin_new(t_floatarg freq){
	t_mind_sin *x = (t_mind_sin*) pd_new(mind_sin_class);
	x->out = outlet_new(&x->x_obj, &s_float);
	x->x_f = freq;
	x->data = (sine_data*) malloc(sizeof(sine_data));
	x->data->sine_table = create_sine_table(x->data->sine_table, freq);
	x->data->index=0;

	//x->tab = malloc(TABLE_SIZE * sizeof(float));
    //int i;
      //  post("pintao");
    //for(i=0; i<TABLE_SIZE; i++){
      //  x->tab[i] = sin(2.0*PI*i*100/TABLE_SIZE);
       // post("%d   %f",i,x->tab[i]);
    //}

	//post("pinto");
	return (void *)x;
}

static void mind_sin_destroy(t_mind_sin *x){
	post("%d", pthread_cancel(x->thread));

}
void *mind_sin_thread (void * arg){
	t_mind_sin *x = (t_mind_sin*) arg;
	float sample;
	//int n=0;
	while(1){
	sample = get_interpolated_freq(x->data,x->x_f ,512); 
	//post("%f",sample);
	//sample = x->tab[n];
	//n=(n+1)%512;
	outlet_float(x->out,sample);	
	//sleep(1);
	nano_sleep(1.0/512.0 * 1e9);
	}
	return 0;
}

static void mind_sin_bang (t_mind_sin *x){
	pthread_create(&x->thread, NULL, mind_sin_thread, x);
}

static void mind_sin_float(t_mind_sin *x, t_floatarg freq){
	x->x_f = freq;
}

void mind_sin_setup(void){
	mind_sin_class = class_new(gensym("mind_sin"), (t_newmethod) mind_sin_new,(t_method) mind_sin_destroy , sizeof(t_mind_sin),CLASS_DEFAULT, A_DEFFLOAT, 0);
	class_addfloat(mind_sin_class,mind_sin_float);
	class_addbang(mind_sin_class, mind_sin_bang);
	class_addmethod(mind_sin_class, (t_method) mind_sin_destroy, gensym("stop"),0);
}
