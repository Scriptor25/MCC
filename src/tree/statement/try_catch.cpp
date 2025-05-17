#include <mcc/builder.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::TryCatchStatement::TryCatchStatement(
    SourceLocation where,
    StatementPtr try_,
    StatementPtr catch_,
    std::string variable)
    : Statement(std::move(where)),
      Try(std::move(try_)),
      Catch(std::move(catch_)),
      Variable(std::move(variable))
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

void mcc::TryCatchStatement::Generate(Builder &builder, const BlockPtr landing_pad) const
{
    const auto parent = builder.GetInsertParent();
    const auto end_target = builder.CreateBlock(parent);
    const auto catch_target = Catch ? builder.CreateBlock(parent) : end_target;

    auto require_end = !Catch;

    Try->Generate(builder, catch_target);

    if (!builder.GetInsertBlock()->GetTerminator())
    {
        require_end = true;
        (void) builder.CreateDirect(end_target);
    }

    if (Catch)
    {
        builder.SetInsertBlock(catch_target);

        if (!Variable.empty())
            (void) builder.CreateStoreResult(Variable);

        Catch->Generate(builder, landing_pad);

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
