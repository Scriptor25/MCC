#include <utility>
#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/statement.hpp>

mcc::TryCatchStatement::TryCatchStatement(
    const SourceLocation &where,
    StatementPtr try_,
    StatementPtr catch_,
    std::string variable,
    TypePtr error_type)
    : Statement(where),
      Try(std::move(try_)),
      Catch(std::move(catch_)),
      Variable(std::move(variable)),
      ErrorType(std::move(error_type))
{
}

std::ostream &mcc::TryCatchStatement::Print(std::ostream &stream) const
{
    Try->Print(stream << "try ");
    if (Catch)
    {
        stream << " catch";
        if (!Variable.empty())
            stream << '(' << Variable << ')';
        Catch->Print(stream << ' ');
    }
    return stream;
}

void mcc::TryCatchStatement::Generate(Builder &builder, Frame &frame) const
{
    const auto parent = builder.GetInsertBlock()->Parent;
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);
    const auto catch_target = Catch ? Block::Create(Catch->Where, builder.GetContext(), parent) : tail_target;

    auto require_tail = !Catch;

    auto target_frame = frame;
    target_frame.LandingPad = catch_target;

    Try->Generate(builder, target_frame);

    if (!builder.GetInsertBlock()->GetTerminator())
    {
        require_tail = true;
        (void) builder.CreateDirect(Try->Where, tail_target);
    }

    if (Catch)
    {
        builder.SetInsertBlock(catch_target);

        builder.PushVariables();

        if (!Variable.empty())
            (void) builder.StoreResult(Catch->Where, ErrorType, Variable);

        Catch->Generate(builder, frame);

        builder.PopVariables();

        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(Catch->Where, tail_target);
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
