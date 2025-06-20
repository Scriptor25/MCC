#include <mcc/constant.hpp>

mcc::Constant::Constant(const SourceLocation &where, const TypePtr &type)
    : Value(where, type, FieldType_Value)
{
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
