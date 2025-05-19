#include <mcc/error.hpp>
#include <mcc/statement.hpp>

mcc::Statement::Statement(const SourceLocation &where)
    : Where(where)
{
}

void mcc::Statement::GenerateInclude(Builder &builder) const
{
    Error(Where, "mcc::Statement::GenerateInclude");
}
