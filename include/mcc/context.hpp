#pragma once

#include <mcc/package.hpp>
#include <mcc/type.hpp>

namespace mcc
{
    struct Context
    {
        Package &Pkg;
        std::string Namespace;
    };
}
