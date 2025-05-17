#include <mcc/error.hpp>
#include <mcc/statement.hpp>

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

void mcc::TryCatchStatement::Generate(Builder &builder) const
{
    Try->Generate(builder);
}
