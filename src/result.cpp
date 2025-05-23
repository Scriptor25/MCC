#include <mcc/common.hpp>

bool mcc::Result::operator==(const Result &result) const
{
    if (Type != result.Type)
        return false;

    switch (Type)
    {
        case ResultType_Value:
            return Value == result.Value;
        case ResultType_Storage:
            return Location == result.Location && Path == result.Path;
        default:
            return false;
    }
}
