#include <utility>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::VariableStatement::VariableStatement(
    const SourceLocation &where,
    const bool constant,
    std::string name,
    TypePtr type,
    ExpressionPtr value)
    : Statement(where),
      Constant(constant),
      Name(std::move(name)),
      Type(std::move(type)),
      Value(std::move(value))
{
}

std::ostream &mcc::VariableStatement::Print(std::ostream &stream) const
{
    stream << (Constant ? "const" : "let") << ' ' << Name;
    if (Type)
        Type->Print(stream << ": ");
    if (Value)
        Value->Print(stream << " = ");
    return stream;
}

void mcc::VariableStatement::Generate(Builder &builder, Frame &frame) const
{
    ValuePtr value;
    if (Value)
    {
        value = Value->GenerateValue(builder, frame);
        Assert(
            (!Type) || (value->Type == Type),
            Where,
            "cannot assign value of type {} to variable of type {}",
            value->Type,
            Type);
    }

    const auto type = Type ? Type : value->Type;
    const auto variable = builder.CreateVariable(Where, type, Name);

    if (value)
    {
        (void) builder.CreateStore(Where, variable, value);
    }
}
