#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::SymbolExpression::SymbolExpression(SourceLocation where, std::string name)
    : Expression(std::move(where)),
      Name(std::move(name))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << Name;
}

mcc::ValuePtr mcc::SymbolExpression::GenerateValue(Builder &builder, const BlockPtr landing_pad) const
{
    return builder.GetVariable(Name);
}
