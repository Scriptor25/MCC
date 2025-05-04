#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ConstantExpression::ConstantExpression(ConstantPtr value, std::string view)
    : Value(std::move(value)),
      View(std::move(view))
{
}

std::ostream &mcc::ConstantExpression::Print(std::ostream &stream) const
{
    return stream << View;
}

mcc::ValuePtr mcc::ConstantExpression::Gen(Builder &builder, const bool inline_) const
{
    return Value;
}
