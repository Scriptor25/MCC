#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::VectorExpression::VectorExpression(
    const SourceLocation &where,
    const std::string &operator_,
    std::vector<ExpressionPtr> operands)
    : Expression(where),
      Operator(operator_),
      Operands(std::move(operands))
{
}

std::ostream &mcc::VectorExpression::Print(std::ostream &stream) const
{
    auto first = true;
    for (auto &operand: Operands)
    {
        if (first)
            first = false;
        else
            stream << ' ' << Operator << ' ';
        operand->Print(stream);
    }
    return stream;
}

mcc::ValuePtr mcc::VectorExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> operands;
    for (auto &operand: Operands)
        operands.emplace_back(operand->GenerateValue(builder, frame));

    auto operator_ = Operator_None;
    if (Operator == "+")
        operator_ = Operator_Add;
    if (Operator == "-")
        operator_ = Operator_Sub;
    if (Operator == "*")
        operator_ = Operator_Mul;
    if (Operator == "/")
        operator_ = Operator_Div;
    if (Operator == "%")
        operator_ = Operator_Rem;

    if (operator_)
        return builder.CreateOperation(Where, operator_, operands);

    Error(Where, "undefined binary operator {}", Operator);
}
