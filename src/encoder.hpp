#pragma once
#include "buffer.hpp"

struct Encoder{
    virtual buffer encode(buffer a)=0;
    virtual buffer decode(buffer a)=0;
    virtual ~Encoder(){}
};

struct MultiEncoder:public Encoder{
    Encoder** encoders;
    size_t num_encoders;
    template<typename...Ts>
    MultiEncoder(Encoder* a,Ts*...args){
        num_encoders=sizeof...(Ts)+1;
        encoders=new Encoder*[sizeof...(Ts)+1]{a,args...};
    }
    ~MultiEncoder(){
        for(size_t n=0;n<num_encoders;n++){
            delete encoders[n];
        }
        delete [] encoders;
    }
    buffer encode(buffer data){
        buffer tmp;
        data=encoders[num_encoders-1]->encode(data);
        for(int n=num_encoders-2;n>=0;n--){
            tmp=encoders[n]->encode(data);
            data.destroy();
            data=tmp;
        }
        return data;
    }
    buffer decode(buffer data){
        buffer tmp;
        data=encoders[0]->decode(data);
        for(int n=1;n<num_encoders;n++){
            tmp=encoders[n]->decode(data);
            data.destroy();
            data=tmp;
        }
        return data;
    }
};

//closest possible thing to a valid 'no change' encoder
struct CopyEncoder:public Encoder{
    buffer encode(buffer data){
        return buffer::copy_of(data);
    }
    buffer decode(buffer data){
        return buffer::copy_of(data);
    }
};
