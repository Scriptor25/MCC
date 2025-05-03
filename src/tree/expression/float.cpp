#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::FloatExpression::FloatExpression(const FloatT value)
    : Value(value)
{
}

std::ostream &mcc::FloatExpression::Print(std::ostream &stream) const
{
    return stream << Value;
}

mcc::ValuePtr mcc::FloatExpression::Gen(Builder &builder, bool inline_) const
{
    return ConstantFloat::Create(Value);
}
