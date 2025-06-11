#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::IfUnlessStatement::IfUnlessStatement(
    const SourceLocation &where,
    const bool unless,
    ExpressionPtr condition,
    StatementPtr then,
    StatementPtr else_)
    : Statement(where),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfUnlessStatement::Print(std::ostream &stream) const
{
    Then->Print(Condition->Print(stream << (Unless ? "unless" : "if") << " (") << ") ");
    if (Else)
        Else->Print(stream << " else ");
    return stream;
}

void mcc::IfUnlessStatement::Generate(Builder &builder, Frame &frame) const
{
    auto condition = Condition->GenerateValue(builder, frame);
    if (!condition->Type->IsNumber())
        condition = builder.CreateNotNull(Condition->Where, condition);

    const auto parent = builder.GetInsertBlock()->Parent;
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);

    auto require_tail = !Else;

    if (const auto constant_condition = std::dynamic_pointer_cast<ConstantNumber>(condition))
    {
        if (Unless == !constant_condition->Value)
        {
            Then->Generate(builder, frame);
            if (!builder.GetInsertBlock()->GetTerminator())
            {
                require_tail = true;
                (void) builder.CreateDirect(Where, tail_target);
            }
        }
        else if (Else)
        {
            Else->Generate(builder, frame);
            if (!builder.GetInsertBlock()->GetTerminator())
            {
                require_tail = true;
                (void) builder.CreateDirect(Where, tail_target);
            }
        }

        if (!require_tail)
        {
            tail_target->Erase();
            builder.SetInsertBlock(nullptr);
        }
        else
            builder.SetInsertBlock(tail_target);
        return;
    }

    const auto then_target = Block::Create(Where, builder.GetContext(), parent);
    const auto else_target = Else ? Block::Create(Where, builder.GetContext(), parent) : tail_target;

    (void) builder.CreateBranch(
        Where,
        condition,
        Unless ? else_target : then_target,
        Unless ? then_target : else_target);

    builder.SetInsertBlock(then_target);
    Then->Generate(builder, frame);
    if (!builder.GetInsertBlock()->GetTerminator())
    {
        require_tail = true;
        (void) builder.CreateDirect(Where, tail_target);
    }

    if (Else)
    {
        builder.SetInsertBlock(else_target);
        Else->Generate(builder, frame);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(Where, tail_target);
        }
    }

    if (!require_tail)
    {
        tail_target->Erase();
        builder.SetInsertBlock(nullptr);
    }
    else
        builder.SetInsertBlock(tail_target);
}
