#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
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
    const auto parent = builder.GetInsertBlock()->Parent;
    auto then_target = Block::Create(Then->Where, builder.GetContext(), parent);
    auto else_target = Block::Create(Else->Where, builder.GetContext(), parent);
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);

    const auto condition = Condition->GenerateValue(builder, frame);
    (void) builder.CreateBranch(
        Where,
        condition,
        Unless ? else_target : then_target,
        Unless ? then_target : else_target);

    builder.SetInsertBlock(then_target);
    const auto then_value = Then->GenerateValue(builder, frame);
    then_target = builder.GetInsertBlock();

    builder.SetInsertBlock(else_target);
    const auto else_value = Else->GenerateValue(builder, frame);
    else_target = builder.GetInsertBlock();

    builder.SetInsertBlock(tail_target);
    const auto type = (then_value->Type == else_value->Type)
                          ? then_value->Type
                          : builder.GetContext().GetUnion({ then_value->Type, else_value->Type });
    auto branch_result = builder.CreateBranchResult(Where, type);

    builder.SetInsertBlock(then_target);
    (void) builder.CreateDirect(Then->Where, tail_target, then_value, branch_result);

    builder.SetInsertBlock(else_target);
    (void) builder.CreateDirect(Else->Where, tail_target, else_value, branch_result);

    builder.SetInsertBlock(tail_target);
    return branch_result;
}
