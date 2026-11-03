#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <dlfcn.h>
void *T_active,*T_cache,*T_freelist_head,*g_log; int T_stdout;
void logRecord_mt(void){} void exit_function(void){abort();}
int main(int argc,char**argv){
 void*h=dlopen(argv[1],RTLD_NOW);if(!h){puts(dlerror());return 1;}
 void(*f)(int16_t*,int16_t*,uint8_t)=dlsym(h,"idft512");
 int16_t*x=aligned_alloc(64,2048),*y=aligned_alloc(64,2048);
 for(int test=0;test<2;test++){
 for(int i=0;i<1024;i++)x[i]=y[i]=0;
 x[test?14:0]=16384;f(x,y,1);
 double err=0,energy=0,ref=16384/sqrt(512.0);
 for(int i=0;i<512;i++){double re=ref*cos(2*M_PI*(test?7:0)*i/512),im=ref*sin(2*M_PI*(test?7:0)*i/512);err+=(y[2*i]-re)*(y[2*i]-re)+(y[2*i+1]-im)*(y[2*i+1]-im);energy+=re*re+im*im;}
 printf("idft512 tone=%d relative_RMS_error=%g firstIQ=%d,%d\n",test?7:0,sqrt(err/energy),y[0],y[1]);
 }return 0;
}