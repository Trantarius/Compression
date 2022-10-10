#include "compressors.hpp"
#include "serial.hpp"

buffer RunLenEncoder::encode(buffer data){
    SerialData<bool,u64> compdata(data.size);
    compdata.meta<1>()=data.size;//uncompressed size, may differ from SerialData's own size record
    buffer comp=compdata.section(0);

    size_t r_i=0;
    for(size_t d_i=0;d_i<data.size;d_i++){
        if(d_i<data.size-2 && data[d_i]==data[d_i+1]){
            //count number of repeated bytes
            int rlen=2;
            for(;rlen<255 && d_i+rlen<data.size && data[d_i+rlen]==data[d_i];rlen++);

            if(r_i>comp.size-3){
                r_i=0;
                break;
            }
            comp[r_i++]=data[d_i];
            comp[r_i++]=data[d_i];
            comp[r_i++]=rlen;
            d_i+=rlen-1;
        }else{
            if(r_i>comp.size-1){
                r_i=0;
                break;
            }
            comp[r_i++]=data[d_i];
        }
    }

    if(r_i==0){
        //compression failed, just copy instead
        compdata.meta<0>()=false;
        memcpy(comp.ptr,data.ptr,data.size);
        return compdata.as_buffer();
    }

    compdata.meta<0>()=true;
    compdata.shrink(data.size-r_i);
    return compdata.as_buffer();
}

buffer RunLenEncoder::decode(buffer data){
    SerialData<bool,u64> compdata(data);
    if(!compdata.meta<0>()){
        //data is actually uncompressed
        return buffer::copy_of(compdata.section(0));
    }

    buffer ret(compdata.meta<1>());
    buffer comp = compdata.section(0);

    size_t r_i=0;
    for(size_t c_i=0;c_i<comp.size;c_i++){
        if( c_i<comp.size-2 && comp[c_i]==comp[c_i+1] ){
            if(r_i+comp[c_i+2]>ret.size){
                throw std::runtime_error("RunLen: uncompressed data too large");
            }
            for(size_t n=0;n<comp[c_i+2];n++){
                ret[r_i++]=comp[c_i];
            }
            c_i+=2;
        }else{
            if(r_i>=ret.size){
                throw std::runtime_error("RunLen: uncompressed data too large");
            }
            ret[r_i++]=comp[c_i];
        }
    }

    return ret;
}
