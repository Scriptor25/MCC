#include <utility>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::ConstantExpression::ConstantExpression(
    const SourceLocation &where,
    ConstantPtr value,
    std::string view)
    : Expression(where),
      Value(std::move(value)),
      View(std::move(view))
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
