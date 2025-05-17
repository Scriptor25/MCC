#include <mcc/common.hpp>

bool mcc::Result::operator==(const Result &result) const
{
    if (Type != result.Type)
        return false;

    switch (Type)
    {
        case ResultType_None:
            return true;
        case ResultType_Value:
            return Value == result.Value;
        case ResultType_Storage:
            return Location == result.Location && Path == result.Path;
        case ResultType_Score:
            return Player == result.Player && Objective == result.Objective;
    }

    return false;
}
