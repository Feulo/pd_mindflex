#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
static t_class *make_folder_class;

typedef struct _make_folder{
	t_object x_obj;
} t_make_folder;


void *make_folder_new(){
	t_make_folder *x = (t_make_folder*) pd_new(make_folder_class);
	return (void *)x;
}

static void make_folder_set(t_make_folder* x, t_symbol *arg){
	 
	mkdir(arg->s_name,0777);
}

void make_folder_setup(void){
	make_folder_class = class_new(gensym("make_folder"), (t_newmethod) make_folder_new, 0, sizeof(t_make_folder),CLASS_DEFAULT, 0);
	class_addsymbol(make_folder_class,make_folder_set);
}
