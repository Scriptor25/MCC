#include <mcc/tree.hpp>

mcc::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

mcc::Expression::Expression(SourceLocation where)
    : Where(std::move(where))
{
}
