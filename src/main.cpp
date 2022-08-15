 
#include "../Util/util.hpp"
#include "runlen.hpp"
#include "serial.hpp"
#include "encoder.hpp"
#include <filesystem>

using namespace std::filesystem;

int main(){
    Encoder* enc=new RunLenEncoder();
    Timer timer;

    directory_iterator dir("Samples");
    for(const directory_entry& file : dir){
        if(file.is_regular_file()){
            timer.start();
            bloc sample=readfile(file.path());
            double t=timer.stop();
            print(file.path().c_str());
            print();
            print("\t",(sample.size));
            print("\t",hexstr(sample.hash()));
            print("\t",Timer::format(t));
            print();
            timer.start();
            bloc comp=enc->encode(sample);
            t=timer.stop();
            print("\t",(comp.size));
            print("\t",hexstr(comp.hash()));
            print("\t",Timer::format(t));
            print();
            timer.start();
            bloc decomp=enc->decode(comp);
            t=timer.stop();
            print("\t",(decomp.size));
            print("\t",hexstr(decomp.hash()));
            print("\t",Timer::format(t));
            print();
            sample.destroy();
            comp.destroy();
            decomp.destroy();
        }
    }
}
