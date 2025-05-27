#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>

mcc::DeleteStatement::DeleteStatement(const SourceLocation &where, ExpressionPtr value)
    : Statement(where),
      Value(std::move(value))
{
}

std::ostream &mcc::DeleteStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "delete ");
}

void mcc::DeleteStatement::Generate(Builder &builder, Frame &frame) const
{
    const auto value = Value->GenerateValue(builder, frame);
    (void) builder.CreateDelete(Where, value);
}
