#include <utility>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::VariableStatement::VariableStatement(
    const SourceLocation &where,
    const bool is_constant,
    std::vector<std::string> names,
    TypePtr type,
    ExpressionPtr value)
    : Statement(where),
      IsConstant(is_constant),
      Names(std::move(names)),
      Type(std::move(type)),
      Value(std::move(value))
{
}

std::ostream &mcc::VariableStatement::Print(std::ostream &stream) const
{
    stream << (IsConstant ? "const" : "let") << ' ';
    for (unsigned i = 0; i < Names.size(); ++i)
    {
        if (i)
        {
            stream << ", ";
        }
        stream << Names.at(i);
    }
    if (Type)
    {
        Type->Print(stream << ": ");
    }
    if (Value)
    {
        Value->Print(stream << " = ");
    }
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

    for (auto &name: Names)
    {
        (void) builder.CreateVariable(Where, type, name, IsConstant, value);
    }
}
