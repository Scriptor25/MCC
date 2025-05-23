#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::ReturnStatement::ReturnStatement(const SourceLocation &where, ExpressionPtr value)
    : Statement(where),
      Value(std::move(value))
{
}

std::ostream &mcc::ReturnStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "return ");
}

void mcc::ReturnStatement::Generate(Builder &builder, Frame &frame) const
{
    if (!Value)
    {
        (void) builder.CreateReturnVoid(Where);
        return;
    }

    const auto value = Value->GenerateValue(builder, frame);
    (void) builder.CreateReturn(Where, value);
}
