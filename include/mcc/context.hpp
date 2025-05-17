#pragma once

#include <mcc/package.hpp>

namespace mcc
{
    struct Context
    {
        Package &Pkg;
        std::string Namespace;
    };
}
