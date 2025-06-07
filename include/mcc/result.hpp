#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    struct Result
    {
        bool operator==(const Result &result) const;

        E_ResultType Type;

        bool WithArgument = false;

        /* Value */
        std::string Value;
        bool NotNull;

        /* Reference */
        E_ReferenceType ReferenceType;
        std::string Target;
        std::string Path;

        /* Argument */
        std::string Name;
    };
}
