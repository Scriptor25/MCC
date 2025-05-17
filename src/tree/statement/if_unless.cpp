#include <mcc/builder.hpp>
#include <mcc/value.hpp>
#include <mcc/tree.hpp>

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

void mcc::IfUnlessStatement::Generate(Builder &builder) const
{
    const auto parent = builder.GetInsertParent();
    const auto end_target = builder.CreateBlock(parent);
    auto then_target = builder.CreateBlock(parent);
    auto else_target = Else ? builder.CreateBlock(parent) : end_target;

    auto require_end = !Else;

    const auto condition = Condition->GenerateValue(builder);
    (void) builder.CreateBranch(
        condition,
        Unless ? else_target : then_target,
        Unless ? then_target : else_target);

    builder.SetInsertBlock(then_target);
    Then->Generate(builder);
    then_target = builder.GetInsertBlock();
    if (!then_target->GetTerminator())
    {
        require_end = true;
        (void) builder.CreateDirect(end_target);
    }

    if (Else)
    {
        builder.SetInsertBlock(else_target);
        Else->Generate(builder);
        else_target = builder.GetInsertBlock();
        if (!else_target->GetTerminator())
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
