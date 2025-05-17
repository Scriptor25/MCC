#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/tree.hpp>
#include <mcc/value.hpp>

mcc::VectorExpression::VectorExpression(
    SourceLocation where,
    std::string operator_,
    std::vector<ExpressionPtr> operands)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
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

bool mcc::VectorExpression::IsConstant() const
{
    for (auto &operand: Operands)
        if (!operand->IsConstant())
            return false;
    return true;
}

bool mcc::VectorExpression::IsNull() const
{
    if (IsConstant())
        return Evaluate() == 0.0;
    return Expression::IsNull();
}

mcc::ValuePtr mcc::VectorExpression::GenerateValue(Builder &builder) const
{
    std::vector<ValuePtr> operands;
    for (auto &operand: Operands)
        operands.emplace_back(operand->GenerateValue(builder));

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
        return builder.CreateOperation(operator_, std::move(operands));

    Error("undefined binary operator {}", Operator);
}
