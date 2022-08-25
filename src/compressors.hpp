#pragma once
#include "Util/util.hpp"
#include "encoder.hpp"

struct RunLenEncoder:public Encoder{
    bloc encode(bloc data);
    bloc decode(bloc data);
};

struct HuffmanEncoder:public Encoder{
    bloc encode(bloc data);
    bloc decode(bloc data);
};
