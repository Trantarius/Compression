#pragma once
#include "../Util/util.hpp"
#include "encoder.hpp"

struct RunLenEncoder{
    bloc encode(bloc data);
    bloc decode(bloc data);
};
