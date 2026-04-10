#include <mcc/error.hpp>
#include <mcc/expression.hpp>

mcc::Expression::Expression(const SourceLocation &where)
    : Statement(where)
{
}

void mcc::Expression::Generate(
        Builder &builder,
        Frame &frame) const
{
    (void) GenerateValue(builder, frame);
}

mcc::FunctionPtr mcc::Expression::GenerateCallee(
        Builder &builder,
        const ParameterRefList &parameters) const
{
    Error(Where, "expression is not callable");
}
