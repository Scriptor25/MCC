#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::IfUnlessExpression::IfUnlessExpression(const bool unless, ExpressionPtr condition, ExpressionPtr then)
    : Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then))
{
}

std::ostream &mcc::IfUnlessExpression::Print(std::ostream &stream) const
{
    return Then->Print(Condition->Print(stream << "if (") << ") ");
}

mcc::ValuePtr mcc::IfUnlessExpression::Gen(Builder &builder, const bool inline_) const
{
    const auto condition = Condition->Gen(builder, inline_);
    const auto then = Then->Gen(builder, true);
    return builder.CreateIf(Unless, condition, then, inline_);
}
