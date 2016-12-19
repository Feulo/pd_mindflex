#include "m_pd.h"  
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "ThinkGearStreamParser.h"     //parser fornecido pela NeuroSky

#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include<pthread.h>

#include <unistd.h>
#include <stdio.h>
 
static t_class *mindflex_class;  
 
typedef struct _mindflex {  
	
	t_object  x_obj;  
	t_outlet *out;
	t_outlet *out_med;
	t_outlet *out_att;
	t_outlet *out_eeg_bands[8];

	pthread_t mindflex_thread, search_thread;
 
	char** addrs; 
	t_int bt_socket;
	ThinkGearStreamParser parser;	
  	const char *dev;
	unsigned char buf;	  
} t_mindflex;  


void handleDataValueFunc(unsigned char extendedCodeLevel, unsigned char code, unsigned char valueLength, const unsigned char *value, void *customData){

	t_mindflex *x = (t_mindflex *) customData;
	t_int rawData;

	if( extendedCodeLevel == 0 ) {
		switch( code ) {
			case( 0x04 ):    /* [CODE]: ATTENTION eSense */
                		outlet_float(x->out_att,(t_int) value[0]);
                        	break;

			case( 0x05 ):   /* [CODE]: MEDITATION eSense */
				outlet_float(x->out_med,(t_int) value[0]);       
                        	break;
                	case( 0x80 ):   // raw data aki pra gente
                       		rawData = value[0]*256 + value[1];
				if( rawData >= 32768 ) rawData = rawData - 65536;
				outlet_float(x->out,rawData);
                               	break;

			case( 0x02 ): //agora a informação de qualidade de sinal
			//	post( "POOR SIGNAL: %d\n", value[0] & 0xFF );
				break;
			case( 0x01 ):
			//	post( "Battery: %d\n", value[0] & 0xFF );
				break;
			case(0x83):
				for(int i=0; i<valueLength; i=i+3){
					int power =0;
					power = (value[i]<<16) | (value[i+1]<<8) |value[i+2];
					outlet_float(x->out_eeg_bands[i/3],power);
			}		
				break;
			default:        /* Other [CODE]s */
                		post( "EXCODE level: %d CODE: 0x%02X vLength: %d\n", extendedCodeLevel, code, valueLength );
                        }
	}

} 


void * mindflex_thread_function(void * arg) {

	t_mindflex *x = (t_mindflex *) arg;

	while(read(x->bt_socket, &(x->buf), sizeof(x->buf))){
		
		THINKGEAR_parseByte( &(x->parser),x-> buf);
        }
	post("saiu do while");	
	return 0;
}

void * bt_search(void *arg){

	t_mindflex *x = (t_mindflex *) arg;
	post("procurando dispositivos...\n");
	int dev_id = hci_get_route(NULL); //comunicacao com placa bluetooth
        int sock = hci_open_dev( dev_id );
        if (dev_id < 0 || sock < 0) {
              perror("opening socket");
              exit(1);
        }

        int len  = 8;
        int max_rsp = 255;
        int flags = IREQ_CACHE_FLUSH;
        inquiry_info *ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

        int num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
        if( num_rsp < 0 ) perror("hci_inquiry");

        x-> addrs = (char**)malloc(num_rsp*sizeof(char*));

        char addr_1[19] = { 0 };
        char name[248] = { 0 };
        for (int i = 0; i < num_rsp; i++) {
              ba2str(&(ii+i)->bdaddr, addr_1);
              memset(name, 0, sizeof(name));
              if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
         	     strcpy(name, "[unknown]");
	 	x->addrs[i] = (char*)malloc(19*sizeof(char));
         	memcpy(x->addrs[i], addr_1, 19);
         	post("%d) %s  %s", i,  addr_1, name);
        }
	post("\nbusca concuída");
	return 0;
}

static void mindflex_activate (t_mindflex *x){
	
	const char command[12]={0x00, 0xF8, 0x00, 0x00, 0x00, 0xE0};
	int n =  write(x->bt_socket, &command,6);
	if(n< 6) post("Activation failed",n);
	else post("Activated");

}
static void mindflex_search(t_mindflex *x){
	pthread_create(&x->search_thread, NULL, bt_search,x);
}

static void mindflex_bang(t_mindflex *x){
	pthread_create(&x->mindflex_thread, NULL, mindflex_thread_function, x);
}

static void mindflex_stop(t_mindflex *x){
	close(x->bt_socket);
}
void* num_connect(void* arg){
	t_mindflex* x = (t_mindflex*) arg;	
	x->bt_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  	struct sockaddr_rc addr;
  	addr.rc_family = AF_BLUETOOTH;
  	addr.rc_channel = (uint8_t)1;
  	//const char* dest = x->addrs[x->num];//"10:14:07:01:11:67";
  	str2ba(x->dev, &addr.rc_bdaddr);
  	int status = connect(x->bt_socket, (struct sockaddr *) &addr, sizeof(addr));
 	post("status: %d",status);
	return 0;
}
static void mindflex_connect(t_mindflex *x, t_float num){
	x->dev= x->addrs[(int) num];
	pthread_create(&x->search_thread, NULL, num_connect,x);
}

static void mindflex_connect_dev(t_mindflex *x, t_symbol *arg){

 	char* tmp = (char*)malloc(19*sizeof(char));
       	memcpy(tmp , arg->s_name, 19);
	x->dev=tmp;
	pthread_create(&x->search_thread, NULL, num_connect,x);
}
void *mindflex_new()  {  
	t_mindflex *x = (t_mindflex *)pd_new(mindflex_class);  
 
	x->out =  outlet_new(&x->x_obj, &s_float);  
	x->out_att = outlet_new(&x->x_obj, &s_float);
	x->out_med = outlet_new(&x->x_obj, &s_float); 
	for(int i=0;i<8;i++)
		x->out_eeg_bands[i]= outlet_new(&x->x_obj,&s_float);

	THINKGEAR_initParser(&(x->parser), PARSER_TYPE_PACKETS,handleDataValueFunc,x);

 return (void *)x;  
}  
 
void mindflex_setup(void) {  
	mindflex_class = class_new(gensym("mindflex"), (t_newmethod)mindflex_new, 0, sizeof(t_mindflex), CLASS_DEFAULT, 0);  

	 class_addbang(mindflex_class, mindflex_bang);

	class_addmethod(mindflex_class, (t_method) mindflex_connect_dev, gensym("connect_dev"),A_DEFSYMBOL,0);
	class_addmethod(mindflex_class, (t_method) mindflex_connect, gensym("connect"),A_DEFFLOAT,0);
	class_addmethod(mindflex_class, (t_method) mindflex_search, gensym("search"),0);
	class_addmethod(mindflex_class, (t_method) mindflex_stop, gensym("stop"),0);
	class_addmethod(mindflex_class, (t_method) mindflex_activate, gensym("activate"),0);

}
