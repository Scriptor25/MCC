#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::UnaryExpression::UnaryExpression(const SourceLocation &where, std::string operator_, ExpressionPtr operand)
    : Expression(where),
      Operator(std::move(operator_)),
      Operand(std::move(operand))
{
}

std::ostream &mcc::UnaryExpression::Print(std::ostream &stream) const
{
    return Operand->Print(stream << Operator);
}

using UnaryOperation = std::function<mcc::ValuePtr(
    const mcc::SourceLocation &where,
    const mcc::Builder &builder,
    const mcc::ValuePtr &operand)>;

struct UnaryOperator
{
    bool Store;
    UnaryOperation Operation;
};

static mcc::ValuePtr inc(const mcc::SourceLocation &where, const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto one = mcc::ConstantNumber::Create(where, builder.GetContext(), 1);
    return builder.CreateOperation(where, mcc::Operator_Add, { operand, one });
};

static mcc::ValuePtr dec(const mcc::SourceLocation &where, const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto one = mcc::ConstantNumber::Create(where, builder.GetContext(), 1);
    return builder.CreateOperation(where, mcc::Operator_Sub, { operand, one });
};

static mcc::ValuePtr neg(const mcc::SourceLocation &where, const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    if (const auto constant_operand = std::dynamic_pointer_cast<mcc::ConstantNumber>(operand))
        return mcc::ConstantNumber::Create(where, builder.GetContext(), -constant_operand->Value);

    const auto zero = mcc::ConstantNumber::Create(where, builder.GetContext(), 0);
    return builder.CreateOperation(where, mcc::Operator_Sub, { zero, operand });
};

mcc::ValuePtr mcc::UnaryExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    static const std::map<std::string_view, UnaryOperator> operators{
        { "++", { true, inc } },
        { "--", { true, dec } },
        { "-", { false, neg } },
    };

    const auto operand = Operand->GenerateValue(builder, frame);
    Assert(operand->Type->IsNumber(), operand->Where, "operand must be of type number, but is {}", operand->Type);

    Assert(operators.contains(Operator), Where, "undefined unary operator {}", Operator);
    auto [store_, operation_] = operators.at(Operator);

    auto result = operation_(Where, builder, operand);

    if (store_)
        return builder.CreateStore(Where, operand, result);

    return result;
}
