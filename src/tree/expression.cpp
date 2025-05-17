#include <mcc/error.hpp>
#include <mcc/expression.hpp>

mcc::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

void mcc::Expression::Generate(Builder &builder, const BlockPtr landing_pad) const
{
    (void) GenerateValue(builder, landing_pad);
}
