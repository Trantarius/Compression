#pragma once
#include "Util/util.hpp"
struct buffer{
    uchar* ptr=NULL;
    size_t size=0;

    buffer(){}
    buffer(size_t size):ptr(new uchar[size]),size(size){}
    buffer(uchar* ptr, size_t size):ptr(ptr),size(size){}
    buffer(const buffer& b):ptr(b.ptr),size(b.size){}
    buffer(const bloc<uchar>& b):ptr(b.ptr),size(b.size){}

    uchar* operator &(){return ptr;}
    const uchar* operator &() const {return ptr;}
    operator uchar*(){return ptr;}
    operator const uchar*() const {return ptr;}
    uchar& operator [] (long n){return (ptr)[(n%size+size)%size];}
    const uchar& operator [] (long n) const {return (ptr)[(n%size+size)%size];}

    void destroy(){
        delete [] ptr;
    }

    void copy(const buffer& b){
        memcpy(ptr,b.ptr,size>b.size?b.size:size);
    }

    static buffer copy_of(const buffer& b){
        uchar* ptr=new uchar[b.size];
        memcpy(ptr,b,b.size);
        return buffer(ptr,b.size);
    }

    u64 hash() const{
        u64 h=0;
        uchar off=0;
        for(size_t n=0;n<size;n++){
            h^=((u64)ptr[n])<<off;
            off=(off+8)%57;
        }
        return h;
    }
};
