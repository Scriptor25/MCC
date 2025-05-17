#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::ForStatement::ForStatement(
    const SourceLocation &where,
    StatementPtr prefix,
    ExpressionPtr condition,
    StatementPtr suffix,
    StatementPtr do_)
    : Statement(where),
      Condition(std::move(condition)),
      Prefix(std::move(prefix)),
      Suffix(std::move(suffix)),
      Do(std::move(do_))
{
}

std::ostream &mcc::ForStatement::Print(std::ostream &stream) const
{
    return Do->Print(Suffix->Print(Condition->Print(Prefix->Print(stream << "for (") << ", ") << ", ") << ") ");
}

void mcc::ForStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto parent = builder.GetInsertParent(Where);
    const auto head_target = builder.CreateBlock(Where, parent);
    const auto loop_target = builder.CreateBlock(Do->Where, parent);
    const auto tail_target = builder.CreateBlock(Where, parent);

    auto target_frame = frame;
    target_frame.Head = head_target;
    target_frame.Tail = tail_target;

    if (Prefix)
        Prefix->Generate(builder, frame);
    (void) builder.CreateDirect(Where, head_target);

    builder.SetInsertBlock(head_target);
    if (Condition)
    {
        const auto condition = Condition->GenerateValue(builder, frame);
        (void) builder.CreateBranch(Condition->Where, condition, loop_target, tail_target);
    }
    else
    {
        (void) builder.CreateDirect(Where, loop_target);
    }

    builder.SetInsertBlock(loop_target);
    Do->Generate(builder, target_frame);
    if (!builder.GetInsertBlock()->GetTerminator())
    {
        if (Suffix)
            Suffix->Generate(builder, frame);
        (void) builder.CreateDirect(Do->Where, head_target);
    }

    if (!Condition && !(target_frame.Flags & FrameFlag_RequireTail))
    {
        builder.RemoveBlock(Where, tail_target);
        builder.SetInsertBlock(nullptr);
    }
    else
    {
        builder.SetInsertBlock(tail_target);
    }
}
