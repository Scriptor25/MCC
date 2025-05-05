#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ConstantExpression::ConstantExpression(SourceLocation where, ConstantPtr value, std::string view)
    : Expression(std::move(where)),
      Value(std::move(value)),
      View(std::move(view))
{
}

std::ostream &mcc::ConstantExpression::Print(std::ostream &stream) const
{
    return stream << View;
}

mcc::ValuePtr mcc::ConstantExpression::Generate(Builder &builder, const bool inline_) const
{
    return Value;
}
