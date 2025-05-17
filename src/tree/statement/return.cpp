#include <mcc/builder.hpp>
#include <mcc/value.hpp>
#include <mcc/tree.hpp>

mcc::ReturnStatement::ReturnStatement(SourceLocation where, ExpressionPtr value)
    : Statement(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &mcc::ReturnStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "return ");
}

void mcc::ReturnStatement::Generate(Builder &builder) const
{
    const auto value = Value->GenerateValue(builder);
    (void) builder.CreateReturn(value);
}
