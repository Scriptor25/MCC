#include <mcc/constant.hpp>

mcc::Constant::Constant(const SourceLocation &where, const TypePtr &type)
    : Value(where, type, false)
{
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
