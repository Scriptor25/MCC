#include <mcc/constant.hpp>

mcc::Constant::Constant(const SourceLocation &where)
    : Value(where)
{
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
