#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::UnaryExpression::UnaryExpression(
    SourceLocation where,
    std::string operator_,
    ExpressionPtr operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      Operand(std::move(operand))
{
}

std::ostream &mcc::UnaryExpression::Print(std::ostream &stream) const
{
    return Operand->Print(stream << Operator);
}

using UnaryOperation = std::function<mcc::ValuePtr(const mcc::Builder &builder, const mcc::ValuePtr &operand)>;

struct UnaryOperator
{
    bool Store;
    UnaryOperation Operation;
};

static mcc::ValuePtr inc(const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto one = mcc::ConstantInteger::Create(1);
    return builder.CreateOperation(mcc::Operator_Add, {operand, one});
};

static mcc::ValuePtr dec(const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto one = mcc::ConstantInteger::Create(1);
    return builder.CreateOperation(mcc::Operator_Sub, {operand, one});
};

static mcc::ValuePtr neg(const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto zero = mcc::ConstantInteger::Create(0);
    return builder.CreateOperation(mcc::Operator_Sub, {zero, operand});
};

mcc::ValuePtr mcc::UnaryExpression::GenerateValue(Builder &builder, const BlockPtr landing_pad) const
{
    static const std::map<std::string_view, UnaryOperator> operators
    {
        {"++", {true, inc}},
        {"--", {true, dec}},
        {"-", {false, neg}},
    };

    Assert(operators.contains(Operator), "undefined unary operator {}", Operator);
    const auto &[store_, operation_] = operators.at(Operator);

    auto operand = Operand->GenerateValue(builder, landing_pad);
    auto value = operation_(builder, operand);

    if (store_)
        return builder.CreateStore(std::move(operand), std::move(value));

    return std::move(value);
}
