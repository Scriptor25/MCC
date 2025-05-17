#include <mcc/builder.hpp>
#include <mcc/value.hpp>
#include <mcc/tree.hpp>

mcc::SymbolExpression::SymbolExpression(SourceLocation where, std::string name)
    : Expression(std::move(where)),
      Name(std::move(name))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << Name;
}

bool mcc::SymbolExpression::IsConstant() const
{
    return false;
}

mcc::ValuePtr mcc::SymbolExpression::GenerateValue(Builder &builder) const
{
    return builder.GetVariable(Name);
}
