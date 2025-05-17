#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::UnaryExpression::UnaryExpression(
    const SourceLocation &where,
    const std::string &operator_,
    ExpressionPtr operand)
    : Expression(where),
      Operator(operator_),
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
    const auto one = mcc::ConstantInteger::Create(where, 1);
    return builder.CreateOperation(where, mcc::Operator_Add, {operand, one});
};

static mcc::ValuePtr dec(const mcc::SourceLocation &where, const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto one = mcc::ConstantInteger::Create(where, 1);
    return builder.CreateOperation(where, mcc::Operator_Sub, {operand, one});
};

static mcc::ValuePtr neg(const mcc::SourceLocation &where, const mcc::Builder &builder, const mcc::ValuePtr &operand)
{
    const auto zero = mcc::ConstantInteger::Create(where, 0);
    return builder.CreateOperation(where, mcc::Operator_Sub, {zero, operand});
};

mcc::ValuePtr mcc::UnaryExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    static const std::map<std::string_view, UnaryOperator> operators
    {
        {"++", {true, inc}},
        {"--", {true, dec}},
        {"-", {false, neg}},
    };

    Assert(operators.contains(Operator), Where, "undefined unary operator {}", Operator);
    const auto &[store_, operation_] = operators.at(Operator);

    const auto operand = Operand->GenerateValue(builder, frame);
    const auto value = operation_(Where, builder, operand);

    if (store_)
        return builder.CreateStore(Where, operand, value);

    return value;
}
