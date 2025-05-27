#include <mcc/common.hpp>

bool mcc::Result::operator==(const Result &result) const
{
    if (Type != result.Type)
    {
        return false;
    }

    switch (Type)
    {
        case ResultType_Value:
            return Value == result.Value;
        case ResultType_Reference:
            return ReferenceType == result.ReferenceType && Target == result.Target && Path == result.Path;
        case ResultType_Argument:
            return Name == result.Name;
        default:
            return false;
    }
}
