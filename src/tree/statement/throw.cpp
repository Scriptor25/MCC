#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>

mcc::ThrowStatement::ThrowStatement(SourceLocation where, ExpressionPtr value)
    : Statement(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &mcc::ThrowStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "throw ");
}

void mcc::ThrowStatement::Generate(Builder &builder) const
{
    auto value = Value->GenerateValue(builder);
    (void) builder.CreateThrow(std::move(value));
}
