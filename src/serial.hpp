#pragma once
#include "buffer.hpp"
#include <tuple>
#include <stdexcept>


template<typename...META_Ts>
class SerialData{
    buffer data;
    /*
     * internal format is:
     * u64:total size
     * u64:total metadata size
     * ???:metadata
     * u64:number of sections
     * u64...: size of each section
     * ???:sections of data
     * */
    u64* total_meta_size;
    std::tuple<META_Ts*...> metaptrs;
    u64* _num_sections;
    u64* section_sizes;
    u8* sections_start;

    template<size_t N>
    void init_metaptrs(u8*& run){}
    template<size_t N,typename T,typename...Ts>
    void init_metaptrs(u8*& run){
        std::get<N>(metaptrs)=( typename std::tuple_element<N,decltype(metaptrs)>::type )run;
        run+=sizeof(T);
        init_metaptrs<N+1,Ts...>(run);
    }

public:
    template<typename...SIZE_Ts>
    SerialData(size_t a,SIZE_Ts...sizes){
        u64 m_size=(sizeof(META_Ts)+...+sizeof(u64));
        u64 s_dat_size=sizeof(u64)*(sizeof...(sizes)+2);
        u64 s_size=(a+...+sizes);
        u64 tot_size=sizeof(u64)+m_size+s_dat_size+s_size;
        data=buffer(tot_size);

        u8* run=&data;
        *(u64*)run=tot_size;
        run+=sizeof(u64);

        total_meta_size=(u64*)run;
        *total_meta_size=(sizeof(META_Ts)+...);
        run+=sizeof(u64);

        init_metaptrs<0,META_Ts...>(run);

        _num_sections=(u64*)run;
        *_num_sections=(sizeof...(sizes)+1);
        run+=sizeof(u64);

        section_sizes=(u64*)run;
        u64 s_size_arr[]={a,sizes...};
        for(size_t s=0;s<*_num_sections;s++){
            section_sizes[s]=s_size_arr[s];
        }
        run+=sizeof(u64)* *_num_sections;
        sections_start=run;
    }

    SerialData(void* dat,size_t max_size){
        if(*(u64*)dat>max_size){
            throw std::runtime_error("invalid serial data; data size larger than max_size");
        }
        u8* run=(u8*)dat;
        data=buffer(run,*(u64*)dat);
        run+=sizeof(u64);

        total_meta_size=(u64*)run;
        run+=sizeof(u64);
        if(*total_meta_size!=(sizeof(META_Ts)+...)
            ||*total_meta_size*sizeof(u64)+run>(u8*)dat+max_size
        ){
            throw std::runtime_error("invalid serial data; metadata size doesn't match");
        }

        init_metaptrs<0,META_Ts...>(run);

        _num_sections=(u64*)run;
        run+=sizeof(u64);
        if(run+*_num_sections*sizeof(u64)>(u8*)dat+max_size){
            throw std::runtime_error("invalid serial data; too many sections");
        }
        section_sizes=(u64*)run;

        u64 tot=0;
        for(int n=0;n<*_num_sections;n++){
            tot+=*(u64*)run;
            run+=sizeof(u64);
        }
        if(run+tot>(u8*)dat+max_size){
            throw std::runtime_error("invalid serial data; sections too large");
        }
        sections_start=run;
    }
    SerialData(buffer dat):SerialData(dat.ptr,dat.size){}

    template<size_t N>
    typename std::remove_pointer<typename std::tuple_element<N,decltype(metaptrs)>::type>::type& meta(){
        return *std::get<N>(metaptrs);
    }

    size_t metadata_size() {return *total_meta_size;}
    size_t num_sections() {return *_num_sections;}
    size_t total_size() {return data.size;}
    size_t section_size(size_t n) {return section_sizes[n];}

    buffer section(size_t n){
        size_t off=0;
        for(size_t i=0;i<n;i++){
            off+=section_sizes[i];
        }
        return buffer(sections_start+off,section_sizes[n]);
    }

    void shrink(size_t amnt){
        data.size-=amnt;
        section_sizes[*_num_sections-1]-=amnt;
        *(u64*)(data.ptr)-=amnt;
    }

    buffer as_buffer(){
        return data;
    }

    void destroy(){
        data.destroy();
    }
};
