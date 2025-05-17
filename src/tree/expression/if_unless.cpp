#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::IfUnlessExpression::IfUnlessExpression(
    const SourceLocation &where,
    const bool unless,
    ExpressionPtr condition,
    ExpressionPtr then,
    ExpressionPtr else_)
    : Expression(where),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfUnlessExpression::Print(std::ostream &stream) const
{
    return Else->Print(Then->Print(Condition->Print(stream << (Unless ? "unless" : "if") << " (") << ") ") << " else ");
}

mcc::ValuePtr mcc::IfUnlessExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    const auto parent = builder.GetInsertParent(Where);
    const auto then_target = builder.CreateBlock(Then->Where, parent);
    const auto else_target = builder.CreateBlock(Else->Where, parent);
    const auto tail_target = builder.CreateBlock(Where, parent);

    const auto condition = Condition->GenerateValue(builder, frame);
    (void) builder.CreateBranch(
        Where,
        condition,
        Unless ? else_target : then_target,
        Unless ? then_target : else_target);

    builder.SetInsertBlock(tail_target);
    auto branch_result = builder.CreateBranchResult(Where, TypeID_Any);

    builder.SetInsertBlock(then_target);
    const auto then_value = Then->GenerateValue(builder, frame);
    (void) builder.CreateDirect(Then->Where, tail_target, then_value, branch_result);

    builder.SetInsertBlock(else_target);
    const auto else_value = Else->GenerateValue(builder, frame);
    (void) builder.CreateDirect(Else->Where, tail_target, else_value, branch_result);

    builder.SetInsertBlock(tail_target);
    return branch_result;
}
