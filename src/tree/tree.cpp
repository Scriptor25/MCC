#include <mcc/error.hpp>
#include <mcc/tree.hpp>

mcc::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

mcc::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

void mcc::Expression::Generate(Builder &builder) const
{
    (void) GenerateValue(builder);
}

bool mcc::Expression::IsNull() const
{
    Error("mcc::Expression::IsNull");
}

bool mcc::Expression::IsEqual(const ExpressionPtr &other) const
{
    Error("mcc::Expression::IsEqual");
}

mcc::FloatT mcc::Expression::Evaluate() const
{
    Error("mcc::Expression::Evaluate");
}
