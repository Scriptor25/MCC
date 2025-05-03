#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::IfUnlessExpression::IfUnlessExpression(
    const bool unless,
    ExpressionPtr condition,
    ExpressionPtr then,
    ExpressionPtr else_)
    : Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfUnlessExpression::Print(std::ostream &stream) const
{
    return Else->Print(Then->Print(Condition->Print(stream << "if (") << ") ") << " else ");
}

mcc::ValuePtr mcc::IfUnlessExpression::Gen(Builder &builder, const bool inline_) const
{
    const auto condition = Condition->Gen(builder, inline_);
    const auto then = Then->Gen(builder, true);
    const auto else_ = Else->Gen(builder, true);
    return builder.CreateIf(Unless, condition, then, else_, inline_);
}
