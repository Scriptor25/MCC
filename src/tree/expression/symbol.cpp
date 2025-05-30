#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::SymbolExpression::SymbolExpression(const SourceLocation &where, const std::string &name)
    : Expression(where),
      Name(name)
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << Name;
}

mcc::ValuePtr mcc::SymbolExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    return builder.GetVariable(Where, Name);
}
