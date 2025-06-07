#include <utility>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>

mcc::MacroExpression::MacroExpression(const SourceLocation &where, std::string name)
    : Expression(where),
      Name(std::move(name))
{
}

std::ostream &mcc::MacroExpression::Print(std::ostream &stream) const
{
    return stream << '!' << Name;
}

mcc::ValuePtr mcc::MacroExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    Error(Where, "mcc::MacroExpression::GenerateValue");
}
