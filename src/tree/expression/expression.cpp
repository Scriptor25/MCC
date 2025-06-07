#include <mcc/error.hpp>
#include <mcc/expression.hpp>

mcc::Expression::Expression(const SourceLocation &where)
    : Statement(where)
{
}

void mcc::Expression::Generate(Builder &builder, Frame &frame) const
{
    (void) GenerateValue(builder, frame);
}
