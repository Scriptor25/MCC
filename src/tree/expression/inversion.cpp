#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::InversionExpression::InversionExpression(ExpressionPtr value)
    : Value(std::move(value))
{
}

std::ostream &mcc::InversionExpression::Print(std::ostream &stream) const
{
    return Value->Print(stream << '!');
}

mcc::ValuePtr mcc::InversionExpression::Gen(Builder &builder, const bool inline_) const
{
    auto value = Value->Gen(builder, inline_);
    value->Invert = true;
    return value;
}
