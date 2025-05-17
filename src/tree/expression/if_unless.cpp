#include <mcc/builder.hpp>
#include <mcc/value.hpp>
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
    return Else->Print(Then->Print(Condition->Print(stream << "if (") << ") ") << " else ");
}

bool mcc::IfUnlessExpression::IsConstant() const
{
    if (!Condition->IsConstant())
        return false;
    if (Unless == Condition->IsNull())
        return Then->IsConstant();
    return Else->IsConstant();
}

bool mcc::IfUnlessExpression::IsNull() const
{
    if (IsConstant())
        return Evaluate() == 0.0;
    return Expression::IsNull();
}

mcc::ValuePtr mcc::IfUnlessExpression::GenerateValue(Builder &builder) const
{
    const auto parent = builder.GetInsertParent();
    const auto then_target = builder.CreateBlock(parent);
    const auto else_target = builder.CreateBlock(parent);
    const auto end_target = builder.CreateBlock(parent);

    const auto condition = Condition->GenerateValue(builder);
    (void) builder.CreateBranch(condition, then_target, else_target);

    builder.SetInsertBlock(end_target);
    auto landing_pad = builder.CreateBranchResult();

    builder.SetInsertBlock(then_target);
    const auto then_value = Then->GenerateValue(builder);
    (void) builder.CreateDirect(end_target, then_value, landing_pad);

    builder.SetInsertBlock(else_target);
    const auto else_value = Else->GenerateValue(builder);
    (void) builder.CreateDirect(end_target, else_value, landing_pad);

    builder.SetInsertBlock(end_target);
    return landing_pad;
}
