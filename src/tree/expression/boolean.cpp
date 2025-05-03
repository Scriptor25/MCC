#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::BooleanExpression::BooleanExpression(const bool value)
    : Value(value)
{
}

std::ostream &mcc::BooleanExpression::Print(std::ostream &stream) const
{
    return stream << (Value ? "true" : "false");
}

mcc::ValuePtr mcc::BooleanExpression::Gen(Builder &builder, bool inline_) const
{
    return ConstantBoolean::Create(Value);
}
