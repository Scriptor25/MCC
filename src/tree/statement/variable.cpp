#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::VariableStatement::VariableStatement(
    const SourceLocation &where,
    const bool constant,
    const std::string &name,
    ExpressionPtr value)
    : Statement(where),
      Constant(constant),
      Name(name),
      Value(std::move(value))
{
}

std::ostream &mcc::VariableStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << (Constant ? "const" : "let") << ' ' << Name << " = ");
}

void mcc::VariableStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto value = Value->GenerateValue(builder, frame);
    const auto variable = builder.CreateVariable(Where, value->Type, Name);
    (void) builder.CreateStore(Where, variable, value);
}
