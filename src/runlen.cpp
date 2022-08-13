#include "runlen.hpp"
#include "serial.hpp"

bloc RunLenEncoder::encode(bloc data){
    SerialData<bool> compdata=(data.size);
    bloc comp=compdata.section(0);

    size_t r_i=0;
    for(size_t r_d=0;r_d<data.size;r_d++){
        if(r_d<data.size-2 && data[r_d]==data[r_d+1]){
            //count number of repeated bytes
            int rlen=2;
            for(;rlen<255 && r_d+rlen<data.size && data[r_d+rlen]==data[r_d];rlen++);

            if(r_i>comp.size-3){
                r_i=0;
                break;
            }
            comp[r_i++]=data[r_d];
            comp[r_i++]=data[r_d];
            comp[r_i++]=rlen;
            r_d+=rlen;
        }else{
            if(r_i>comp.size-1){
                r_i=0;
                break;
            }
            comp[r_i++]=data[r_d];
        }
    }

    if(r_i==0){
        //compression failed, just copy instead
        compdata.meta<0>()=false;
        memcpy(comp.ptr,data.ptr,data.size);
        return compdata.as_bloc();
    }

    ret.size=r_i;

    return ret;
}
