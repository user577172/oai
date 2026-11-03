#include <uhd/convert.hpp>
#include <cstdio>
#include <cstdint>
#include "usrp_converters.hpp"
int main(){register_oai_converters(4);int16_t src[8]={1,2,-3,4,50,-60,70,80},scaled[8],wire[8],ref[8];for(int i=0;i<8;i++)scaled[i]=src[i]*16;
uhd::convert::id_type a;a.input_format="sc16_oai";a.num_inputs=1;a.output_format="sc16_item32_le";a.num_outputs=1;auto c=uhd::convert::get_converter(a)();c->set_scalar(1);c->conv({src},{wire},4);
a.input_format="sc16";auto d=uhd::convert::get_converter(a)();d->set_scalar(1);d->conv({scaled},{ref},4);
for(int i=0;i<8;i++)printf("%d oai=%d uhd=%d\n",i,wire[i],ref[i]);}
