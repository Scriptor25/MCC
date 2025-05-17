#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::IfUnlessStatement::IfUnlessStatement(
    SourceLocation where,
    const bool unless,
    ExpressionPtr condition,
    StatementPtr then,
    StatementPtr else_)
    : Statement(std::move(where)),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfUnlessStatement::Print(std::ostream &stream) const
{
    Then->Print(Condition->Print(stream << "if (") << ") ");
    if (Else)
        Else->Print(stream << " else ");
    return stream;
}

void mcc::IfUnlessStatement::Generate(Builder &builder, const BlockPtr landing_pad) const
{
    const auto parent = builder.GetInsertParent();
    const auto end_target = builder.CreateBlock(parent);
    const auto then_target = builder.CreateBlock(parent);
    const auto else_target = Else ? builder.CreateBlock(parent) : end_target;

    auto require_end = !Else;

    const auto condition = Condition->GenerateValue(builder, landing_pad);
    (void) builder.CreateBranch(
        condition,
        Unless ? else_target : then_target,
        Unless ? then_target : else_target);

    builder.SetInsertBlock(then_target);
    Then->Generate(builder, landing_pad);
    if (!builder.GetInsertBlock()->GetTerminator())
    {
        require_end = true;
        (void) builder.CreateDirect(end_target);
    }

    if (Else)
    {
        builder.SetInsertBlock(else_target);
        Else->Generate(builder, landing_pad);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_end = true;
            (void) builder.CreateDirect(end_target);
        }
    }

    if (!require_end)
    {
        builder.RemoveBlock(end_target);
        builder.SetInsertBlock(nullptr);
    }
    else
    {
        builder.SetInsertBlock(end_target);
    }
}
