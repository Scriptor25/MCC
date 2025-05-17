#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::ForStatement::ForStatement(
    SourceLocation where,
    StatementPtr prefix,
    ExpressionPtr condition,
    StatementPtr suffix,
    StatementPtr do_)
    : Statement(std::move(where)),
      Prefix(std::move(prefix)),
      Condition(std::move(condition)),
      Suffix(std::move(suffix)),
      Do(std::move(do_))
{
}

std::ostream &mcc::ForStatement::Print(std::ostream &stream) const
{
    return Do->Print(Suffix->Print(Condition->Print(Prefix->Print(stream << "for (") << ", ") << ", ") << ") ");
}

void mcc::ForStatement::Generate(Builder &builder, const BlockPtr landing_pad) const
{
    const auto parent = builder.GetInsertParent();
    const auto loop_target = builder.CreateBlock(parent);
    const auto end_target = builder.CreateBlock(parent);

    Prefix->Generate(builder, landing_pad);
    {
        const auto condition = Condition->GenerateValue(builder, landing_pad);
        (void) builder.CreateBranch(condition, loop_target, end_target);
    }

    builder.SetInsertBlock(loop_target);
    Do->Generate(builder, landing_pad);
    Suffix->Generate(builder, landing_pad);
    {
        const auto condition = Condition->GenerateValue(builder, landing_pad);
        (void) builder.CreateBranch(condition, loop_target, end_target);
    }

    builder.SetInsertBlock(end_target);
}
