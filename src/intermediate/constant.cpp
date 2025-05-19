#include <mcc/constant.hpp>

mcc::Constant::Constant(const SourceLocation &where, TypeContext &context, const TypePtr &type)
    : Value(where, context, type)
{
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
