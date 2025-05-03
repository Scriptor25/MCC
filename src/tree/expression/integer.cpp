#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::IntegerExpression::IntegerExpression(const int64_t value)
    : Value(value)
{
}

std::ostream &mcc::IntegerExpression::Print(std::ostream &stream) const
{
    return stream << Value;
}

mcc::ValuePtr mcc::IntegerExpression::Gen(Builder &builder, bool inline_) const
{
    return ConstantInteger::Create(Value);
}
