#include <mcc/error.hpp>
#include <mcc/expression.hpp>

mcc::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

void mcc::Expression::Generate(Builder &builder) const
{
    (void) GenerateValue(builder);
}
