#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::ConstantExpression::ConstantExpression(const SourceLocation &where, const ConstantPtr &value, const std::string &view)
    : Expression(where), Value(value), View(view)
{
}

std::ostream &mcc::ConstantExpression::Print(std::ostream &stream) const
{
    return stream << View;
}

mcc::ValuePtr mcc::ConstantExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    return Value;
}
