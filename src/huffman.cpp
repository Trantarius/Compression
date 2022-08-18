#include <list>
#include "compressors.hpp"
#include "serial.hpp"
struct bitstream{
    bloc home;
    size_t size;//in bits
    size_t byte=0;
    uchar bit=0;
    bitstream(){}
    bitstream(bloc home):home(home){}

    void next(size_t n=1){
        bit+=n;
        byte+=bit/8;
        bit%=8;
    }
    void reset(){
        byte=0;
        bit=0;
    }

    bool pop(){
        bool ret=(home[byte]<<bit)&128;
        next();
        return ret;
    }
    void set(bool b){
        home[byte]^=((home[byte]<<bit)&128 ^ ((uchar)b<<7))>>bit;
    }
    void push(bool b){
        home[byte]^=((home[byte]<<bit)&128 ^ ((uchar)b<<7))>>bit;
        next();
    }
    void push(bitstream bs){
        bs.reset();
        for(size_t n=0;n<bs.size;n++){
            push(bs.pop());
        }
    }
};


struct huffnode{
    enum{STEM,LEAF} mode=LEAF;
    size_t count=0;
    union{
        huffnode* child[2]{NULL,NULL};
        uchar byte;
    };
    huffnode(){}
    huffnode(huffnode* a,huffnode* b):child{a,b},mode(STEM),count(a->count+b->count){}
    huffnode(uchar b,size_t c):byte(b),mode(LEAF),count(c){}
    void destroy(){
        if(mode==STEM){
            if(child[0]!=NULL){
                child[0]->destroy();
            }
            if(child[1]!=NULL){
                child[1]->destroy();
            }
            delete this;
        }
    }
    uchar get(bitstream& bs){
        if(mode==LEAF){
            return byte;
        }else{
            return child[(uchar)(bs.pop())]->get(bs);
        }
    }
};

bool compare_node(huffnode* a,huffnode* b){
    return a->count<b->count;
}

void flatten_hufftree_r(bitstream* target,huffnode* root,bitstream tracker){
    if(root->mode==huffnode::LEAF){
        target[root->byte].home.copy(tracker.home);
        target[root->byte].size=tracker.byte*8+tracker.bit;
    }else{
        tracker.next();
        tracker.set(0);
        flatten_hufftree_r(target,root->child[0],tracker);
        tracker.set(1);
        flatten_hufftree_r(target,root->child[1],tracker);
    }
}

bitstream* flatten_hufftree(huffnode* root){
    bitstream* target=new bitstream[256];
    for(int n=0;n<256;n++){
        target[n].home=bloc(32);
    }
    bitstream tracker(bloc(32));
    tracker.set(0);
    flatten_hufftree_r(target,root->child[0],tracker);
    tracker.set(1);
    flatten_hufftree_r(target,root->child[1],tracker);
    tracker.home.destroy();
    return target;
}

bloc HuffmanEncoder::encode(bloc data){
    //get frequency data on each byte
    huffnode nodes[256];
    std::list<huffnode*> nodelist;
    for(size_t n=0;n<256;n++){
        nodes[n].byte=n;
        nodelist.push_back(&(nodes[n]));
    }
    for(size_t n=0;n<data.size;n++){
        nodes[data[n]].count++;
    }

    //create hufftree
    nodelist.sort(compare_node);
    while(nodelist.size()>1){
        huffnode* front=nodelist.front();
        nodelist.pop_front();
        huffnode* stem=new huffnode(front,nodelist.front());
        nodelist.pop_front();
        bool broke=false;
        for(auto it=nodelist.begin();it!=nodelist.end();it++){
            if(compare_node(stem,*it)){
                nodelist.insert(it,stem);
                broke=true;
                break;
            }
        }
        if(!broke){
            nodelist.push_back(stem);
        }
    }
    huffnode* root=nodelist.front();

    //flatten into a byte->bitstream lookup table
    bitstream* hufftable=flatten_hufftree(root);

    //put hufftable in the first 2 sections of return data
    size_t total_huff=0;
    for(size_t n=0;n<256;n++){
        total_huff+=hufftable[n].size;
    }
    SerialData<u64> retdata(256,total_huff/8+1,data.size);
    retdata.meta<0>()=data.size;
    bloc s0=retdata.section(0);
    bitstream bs1(retdata.section(1));

    for(int n=0;n<256;n++){
        s0[n]=hufftable[n].size;
        bs1.push(hufftable[n]);
    }

    //compress the data
    bitstream ret(retdata.section(2));
    for(size_t n=0;n<data.size;n++){
        ret.push(hufftable[data[n]]);
    }
    retdata.shrink(data.size-ret.byte-1);

    //cleanup
    root->destroy();
    for(int n=0;n<256;n++){
        hufftable[n].home.destroy();
    }
    delete [] hufftable;

    return retdata.as_bloc();
}

bloc HuffmanEncoder::decode(bloc data){
    return bloc::copy_of(data);
}
