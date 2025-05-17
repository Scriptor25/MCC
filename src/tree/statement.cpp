#include <mcc/statement.hpp>

mcc::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}
