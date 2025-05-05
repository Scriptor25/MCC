#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ReturnExpression::ReturnExpression(SourceLocation where, ExpressionPtr value)
    : Expression(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &mcc::ReturnExpression::Print(std::ostream &stream) const
{
    return Value->Print(stream << "return ");
}

mcc::ValuePtr mcc::ReturnExpression::Generate(Builder &builder, const bool inline_) const
{
    const auto value = Value->Generate(builder, inline_);
    return builder.CreateReturn(value, inline_);
}
