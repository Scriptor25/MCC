#include <mcc/result.hpp>

bool mcc::Result::operator==(const Result &result) const
{
    if (Type != result.Type)
        return false;

    switch (Type)
    {
    case ResultType_::Value:
        return Value == result.Value;
    case ResultType_::Reference:
        return ReferenceType == result.ReferenceType && Target == result.Target && Path == result.Path;
    case ResultType_::Argument:
        return Name == result.Name;
    default:
        return false;
    }
}
