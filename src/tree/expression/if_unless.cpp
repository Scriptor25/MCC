#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

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

mcc::ValuePtr mcc::IfUnlessExpression::GenerateValue(Builder &builder, const BlockPtr landing_pad) const
{
    const auto parent = builder.GetInsertParent();
    const auto then_target = builder.CreateBlock(parent);
    const auto else_target = builder.CreateBlock(parent);
    const auto end_target = builder.CreateBlock(parent);

    const auto condition = Condition->GenerateValue(builder, landing_pad);
    (void) builder.CreateBranch(condition, then_target, else_target);

    builder.SetInsertBlock(end_target);
    auto branch_result = builder.CreateBranchResult();

    builder.SetInsertBlock(then_target);
    const auto then_value = Then->GenerateValue(builder, landing_pad);
    (void) builder.CreateDirect(end_target, then_value, branch_result);

    builder.SetInsertBlock(else_target);
    const auto else_value = Else->GenerateValue(builder, landing_pad);
    (void) builder.CreateDirect(end_target, else_value, branch_result);

    builder.SetInsertBlock(end_target);
    return branch_result;
}
