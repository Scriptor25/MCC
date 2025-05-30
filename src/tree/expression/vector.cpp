#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
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
    for (auto &operand : Operands)
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
    std::vector<ValuePtr> operand_values;
    std::vector<std::shared_ptr<ConstantNumber>> operand_constants;

    for (auto &operand : Operands)
    {
        auto operand_value = operand->GenerateValue(builder, frame);
        Assert(
            operand_value->Type->IsNumber(),
            operand->Where,
            "operand must be of type number, but is {}",
            operand_value->Type);
        operand_values.emplace_back(operand_value);
        if (auto constant = std::dynamic_pointer_cast<ConstantNumber>(operand_value))
            operand_constants.emplace_back(constant);
    }

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

    if (!operator_)
        Error(Where, "undefined binary operator {}", Operator);

    if (operand_values.size() == operand_constants.size())
    {
        auto value = operand_constants.front()->Value;
        for (unsigned i = 1; i < operand_constants.size(); ++i)
        {
            switch (operator_)
            {
            case Operator_Add:
                value += operand_constants[i]->Value;
                break;
            case Operator_Sub:
                value -= operand_constants[i]->Value;
                break;
            case Operator_Mul:
                value *= operand_constants[i]->Value;
                break;
            case Operator_Div:
                value /= operand_constants[i]->Value;
                break;
            case Operator_Rem:
                value %= operand_constants[i]->Value;
                break;
            default:
                break;
            }
        }
        return ConstantNumber::Create(Where, builder.GetContext(), value);
    }

    return builder.CreateOperation(Where, operator_, operand_values);
}
