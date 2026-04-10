#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::VectorExpression::VectorExpression(
        const SourceLocation &where,
        std::string operator_,
        std::vector<ExpressionPtr> operands)
    : Expression(where),
      Operator(std::move(operator_)),
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

mcc::ValuePtr mcc::VectorExpression::GenerateValue(
        Builder &builder,
        const Frame &frame) const
{
    std::vector<ValuePtr> values(Operands.size());
    std::vector<std::shared_ptr<ConstantNumber>> constants(Operands.size());

    auto all_constant = true, all_number = true;
    for (unsigned i = 0; i < Operands.size(); ++i)
    {
        auto value = Operands[i]->GenerateValue(builder, frame);

        values[i] = value;

        all_number &= value->Type->IsNumber();

        if (auto c = std::dynamic_pointer_cast<ConstantNumber>(value))
            constants[i] = std::move(c);
        else
            all_constant = false;
    }

    const auto operator_ = ToOperator(Operator);
    Assert(!!operator_, Where, "undefined binary operator {}", Operator);

    if (all_constant)
    {
        auto value = constants.front()->Value;
        for (unsigned i = 1; i < constants.size(); ++i)
            switch (*operator_)
            {
            case Operator_::Add:
                value += constants[i]->Value;
                break;
            case Operator_::Sub:
                value -= constants[i]->Value;
                break;
            case Operator_::Mul:
                value *= constants[i]->Value;
                break;
            case Operator_::Div:
                value /= constants[i]->Value;
                break;
            case Operator_::Rem:
                value %= constants[i]->Value;
                break;
            }
        return ConstantNumber::Create(Where, builder.GetContext(), value);
    }

    if (all_number)
        return builder.CreateOperation(Where, {}, *operator_, values);

    auto accum = values.front();
    for (unsigned i = 1; i < Operands.size(); ++i)
    {
        const ParameterRefList parameters = {
            {     accum->Type,     accum->FieldType },
            { values[i]->Type, values[i]->FieldType },
        };

        const auto candidates =
                builder.FindFunctions({ "operator", BinaryExpression::MapOperator(Operator) }, parameters, false);
        const auto callee = builder.FindUnambiguousCandidate(Where, candidates, parameters);

        accum = builder.CreateCall(Where, {}, callee, { accum, values[i] }, frame.LandingPad);
    }

    return accum;
}
