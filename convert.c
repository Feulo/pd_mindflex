#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char** argv){


        char c[sizeof(float)];
        unsigned char* d;
        float* temp;
	float sample;
	FILE* fp,*fpw;
	int i;
	
	fp = fopen(argv[1],"r");
	fpw = fopen(argv[2],"w");
	printf("%s\n",argv[1]);
        while(1){
                for(i=0;i<sizeof(float);i++){
                        c[i] = fgetc(fp);
		}
                temp = (float*)c;
		sample = *temp /2048;

		d = (unsigned char*) & sample;
		for(int j =0;j< sizeof(float);j++)
			fputc(d[j],fpw);
	                
               	if(c[0]==EOF){			
			fclose(fp);
			fputc(EOF,fpw);
			fclose(fpw);
               		return 0;
		}
        }        
        return 0;	
}
