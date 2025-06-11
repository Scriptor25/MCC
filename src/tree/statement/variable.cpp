#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::VariableStatement::VariableStatement(
    const SourceLocation &where,
    const E_Declarator declarator,
    const bool is_reference,
    std::vector<std::string> names,
    TypePtr type,
    ExpressionPtr value)
    : Statement(where),
      Declarator(declarator),
      IsReference(is_reference),
      Names(std::move(names)),
      Type(std::move(type)),
      Value(std::move(value))
{
}

std::ostream &mcc::VariableStatement::Print(std::ostream &stream) const
{
    stream << Declarator << (IsReference ? "&" : "") << ' ';
    for (unsigned i = 0; i < Names.size(); ++i)
    {
        if (i)
            stream << ", ";
        stream << Names[i];
    }
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
            !Type || SameOrSpecial(value->Type, Type),
            Where,
            "cannot assign value of type {} to variable of type {}",
            value->Type,
            Type);
    }

    const auto constant = std::dynamic_pointer_cast<Constant>(value);
    const auto type = Type ? Type : value->Type;

    for (auto &name : Names)
        if (IsReference)
            (void) builder.InsertVariable(Where, name, value);
        else
            switch (Declarator)
            {
            case Declarator_Let:
                (void) builder.CreateVariable(Where, type, name, true, value);
                break;
            case Declarator_Const:
                (void) builder.CreateVariable(Where, type, name, false, value);
                break;
            case Declarator_ConstExpr:
                Assert(!!constant, Where, "constant expression initializer must be a constant value");
                builder.InsertVariable(Where, name, value);
                break;
            }
}
