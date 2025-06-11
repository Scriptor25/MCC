#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
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
        auto void_type = builder.GetContext().GetVoid();
        Assert(
            SameOrSpecial(void_type, frame.ResultType),
            Where,
            "cannot return value of type {} for result type {}",
            void_type,
            frame.ResultType);

        return (void) builder.CreateReturn(Where);
    }

    const auto value = Value->GenerateValue(builder, frame);

    Assert(
        SameOrSpecial(value->Type, frame.ResultType),
        Where,
        "cannot return value of type {} for result type {}",
        value->Type,
        frame.ResultType);

    (void) builder.CreateReturn(Where, value);
}
