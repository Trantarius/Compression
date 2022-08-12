 
#include "../Util/util.hpp"
#include "serial.hpp"

int main(){
    SerialData<int> sd(8,9);
    print(sd.num_sections());
    sd.meta<0>() = 99;
    print(sd.meta<0>());
    sd.meta<0>() = 77;
    print(sd.meta<0>());

    bloc s0=sd.section(0);
    for(int n=0;n<s0.size;n++){
        s0[n]=n;
    }
    bloc s0b=sd.section(0);
    for(int n=0;n<s0b.size;n++){
        print(s0b[n]);
    }

    bloc sdb=sd.as_bloc();
    print("as_bloc");

    SerialData<int> sd2(sdb.ptr,sdb.size);
    print("constructed");

    print(sd2.meta<0>());



    s0=sd2.section(0);
    for(int n=0;n<s0.size;n++){
        print(s0[n]);
    }
}
