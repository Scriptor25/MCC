#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>

mcc::ThrowStatement::ThrowStatement(const SourceLocation &where, ExpressionPtr value)
    : Statement(where),
      Value(std::move(value))
{
}

std::ostream &mcc::ThrowStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "throw ");
}

void mcc::ThrowStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto value = Value->GenerateValue(builder, frame);
    (void) builder.CreateThrow(Where, value, frame.LandingPad);
}
