#include <mcc/constant.hpp>

mcc::Constant::Constant(const SourceLocation &where, TypePtr type)
    : Value(where, type)
{
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
