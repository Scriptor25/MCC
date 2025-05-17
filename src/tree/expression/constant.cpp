#include <mcc/constant.hpp>
#include <mcc/tree.hpp>
#include <mcc/value.hpp>

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

bool mcc::ConstantExpression::IsConstant() const
{
    return true;
}

bool mcc::ConstantExpression::IsNull() const
{
    return Value->IsNull();
}

mcc::ValuePtr mcc::ConstantExpression::GenerateValue(Builder &builder) const
{
    return Value;
}
