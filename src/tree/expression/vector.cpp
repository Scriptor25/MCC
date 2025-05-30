#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
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
    std::vector<ValuePtr> operands;
    std::vector<std::shared_ptr<ConstantNumber>> constant_operands;

    for (auto &operand : Operands)
    {
        auto value = operand->GenerateValue(builder, frame);
        operands.emplace_back(value);
        if (auto constant = std::dynamic_pointer_cast<ConstantNumber>(value))
            constant_operands.emplace_back(constant);
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

    if (operands.size() == constant_operands.size())
    {
        auto value = constant_operands.front()->Value;
        for (unsigned i = 1; i < constant_operands.size(); ++i)
        {
            switch (operator_)
            {
            case Operator_Add:
                value += constant_operands[i]->Value;
                break;
            case Operator_Sub:
                value -= constant_operands[i]->Value;
                break;
            case Operator_Mul:
                value *= constant_operands[i]->Value;
                break;
            case Operator_Div:
                value /= constant_operands[i]->Value;
                break;
            case Operator_Rem:
                value %= constant_operands[i]->Value;
                break;
            default:
                break;
            }
        }
        return ConstantNumber::Create(Where, builder.GetContext(), value);
    }

    return builder.CreateOperation(Where, operator_, operands);
}
