#pragma once
#include "buffer.hpp"
#include "encoder.hpp"

struct RunLenEncoder:public Encoder{
    buffer encode(buffer data);
    buffer decode(buffer data);
};

struct HuffmanEncoder:public Encoder{
    buffer encode(buffer data);
    buffer decode(buffer data);
};
