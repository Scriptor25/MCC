#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ReturnExpression::ReturnExpression(ExpressionPtr value)
    : Value(std::move(value))
{
}

std::ostream &mcc::ReturnExpression::Print(std::ostream &stream) const
{
    return Value->Print(stream << "return ");
}

mcc::ValuePtr mcc::ReturnExpression::Gen(Builder &builder, const bool inline_) const
{
    const auto value = Value->Gen(builder, inline_);
    return builder.CreateReturn(value, inline_);
}
