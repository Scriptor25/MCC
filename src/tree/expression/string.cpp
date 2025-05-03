#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::StringExpression::StringExpression(std::string value)
    : Value(std::move(value))
{
}

std::ostream &mcc::StringExpression::Print(std::ostream &stream) const
{
    return stream << '"' << Value << '"';
}

mcc::ValuePtr mcc::StringExpression::Gen(Builder &builder, bool inline_) const
{
    return ConstantString::Create(Value);
}
