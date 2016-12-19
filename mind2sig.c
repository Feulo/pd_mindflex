#include "m_pd.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.14159265
#define TABLE_SIZE 2048

typedef struct t_data{
    float * sine_table;
    float index;
}sine_data;

float *  create_sine_table(float * table){
    table = malloc(TABLE_SIZE * sizeof(float));
    int i;
    for(i=0; i<TABLE_SIZE; i++){
        table[i] = sin(2.0*PI*i/TABLE_SIZE);
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

static t_class *mind2sig_class;

typedef struct _mind2sig{
	t_object x_obj;
	t_outlet *out;
	t_float  sample;
	t_float  x_f;
	sine_data *data;
} t_mind2sig;


static t_int *mind2sig_perform(t_int *w){
	
	t_mind2sig *x = (t_mind2sig*) (w[1]);
	t_float *out = (t_float*) (w[2]);
	int n = (int) (w[3]);
	post("%f",x->sample);
	while(n--){
		*out++= x->sample;
	//	*out++ = get_interpolated_freq(x->data,x->x_f,sys_getsr());
	}
	return (w+4);
}

static void mind2sig_dsp(t_mind2sig *x, t_signal **sp){
	dsp_add(mind2sig_perform, 3, x,sp[0]->s_vec,sp[0]->s_n);
}
void *mind2sig_new(){
	t_mind2sig *x = (t_mind2sig*) pd_new(mind2sig_class);
	x->out = outlet_new(&x->x_obj, &s_signal);
	x->x_f = 400;
	x->sample =0;
	floatinlet_new (&x->x_obj,&x-> sample );

	x->data = (sine_data*) malloc(sizeof(sine_data));
	x->data->sine_table = create_sine_table(x->data->sine_table);
	x->data->index=0;
	return (void *)x;
}




void mind2sig_setup(void){
	mind2sig_class = class_new(gensym("mind2sig"), (t_newmethod) mind2sig_new, 0, sizeof(t_mind2sig),CLASS_DEFAULT,0);
	//class_addfloat(mindflex_filter_class,mindflex_filter_filter);
	class_addmethod(mind2sig_class, (t_method) mind2sig_dsp, gensym("dsp"), A_CANT,0);
}
