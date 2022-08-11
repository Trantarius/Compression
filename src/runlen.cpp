#include "runlen.hpp"

bloc RunLenEncoder::encode(bloc data){
    bloc ret(data.size+1+sizeof(size_t));

    //flags that compression was actually used. if 0, the data is a copy instead.
    ret[0]=1;
    *(size_t*)(&ret+1)=data.size;

    size_t r_i=1+sizeof(size_t);
    for(size_t r_d=0;r_d<data.size;r_d++){
        if(r_d<data.size-2 && data[r_d]==data[r_d+1]){
            //count number of repeated bytes
            int rlen=2;
            for(;rlen<255 && r_d+rlen<data.size && data[r_d+rlen]==data[r_d];rlen++);

            if(r_i>ret.size-3){
                r_i=0;
                break;
            }
            ret[r_i++]=data[r_d];
            ret[r_i++]=data[r_d];
            ret[r_i++]=rlen;
            r_d+=rlen;
        }else{
            if(r_i>ret.size-1){
                r_i=0;
                break;
            }
            ret[r_i++]=data[r_d];
        }
    }

    if(r_i==0){
        //compression failed, just copy instead
    }

    ret.size=r_i;
    return ret;
}
