#include <mcc/builder.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::TryCatchStatement::TryCatchStatement(
    const SourceLocation &where,
    StatementPtr try_,
    StatementPtr catch_,
    const std::string &variable)
    : Statement(where),
      Try(std::move(try_)),
      Catch(std::move(catch_)),
      Variable(variable)
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

void mcc::TryCatchStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto parent = builder.GetInsertParent(Where);
    const auto tail_target = builder.CreateBlock(Where, parent);
    const auto catch_target = Catch ? builder.CreateBlock(Catch->Where, parent) : tail_target;

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

        if (!Variable.empty())
            (void) builder.CreateStoreResult(Catch->Where, TypeID_Any, Variable);

        Catch->Generate(builder, frame);

        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(Catch->Where, tail_target);
        }
    }

    if (!require_tail)
    {
        builder.RemoveBlock(Where, tail_target);
        builder.SetInsertBlock(nullptr);
    }
    else
    {
        builder.SetInsertBlock(tail_target);
    }
}
