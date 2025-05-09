#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::IfUnlessExpression::IfUnlessExpression(
    SourceLocation where,
    const bool unless,
    ExpressionPtr condition,
    ExpressionPtr then,
    ExpressionPtr else_)
    : Expression(std::move(where)),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfUnlessExpression::Print(std::ostream &stream) const
{
    Then->Print(Condition->Print(stream << "if (") << ") ");
    if (Else)
        Else->Print(stream << " else ");
    return stream;
}

mcc::ValuePtr mcc::IfUnlessExpression::GenerateValue(Builder &builder) const
{
    Error(Where, "TODO");
}
