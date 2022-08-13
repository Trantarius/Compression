#pragma once
#include "../Util/util.hpp"

struct Encoder{
    virtual bloc encode(bloc a)=0;
    virtual bloc decode(bloc a)=0;
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
    bloc encode(bloc data){
        bloc tmp;
        data=encoders[num_encoders-1]->encode(data);
        for(int n=num_encoders-2;n>0;n--){
            tmp=encoders[n]->encode(data);
            data.destroy();
            data=tmp;
        }
        return data;
    }
    bloc decode(bloc data){
        bloc tmp;
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
    bloc encode(bloc data){
        return bloc::copy(data);
    }
    bloc decode(bloc data){
        return bloc::copy(data);
    }
};
