#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    struct Result
    {
        bool operator==(const Result &result) const;

        ResultType_ Type;

        bool WithArgument = false;

        /* Value */
        std::string Value;
        bool NotNull;

        /* Reference */
        ReferenceType_ ReferenceType;
        std::string Target;
        std::string Path;

        /* Argument */
        std::string Name;
    };
}
