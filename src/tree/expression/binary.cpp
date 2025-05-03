#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::BinaryExpression::BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right)
    : Operator(std::move(operator_)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return Right->Print(Left->Print(stream) << ' ' << Operator << ' ');
}

mcc::ValuePtr mcc::BinaryExpression::Gen(Builder &builder, const bool inline_) const
{
    const auto left = Left->Gen(builder, inline_);
    const auto right = Right->Gen(builder, inline_);

    if (Operator == "=")
        return builder.CreateStore(left, right, inline_);

    auto comparator = Comparator_None;
    if (Operator == "<")
        comparator = Comparator_LT;
    if (Operator == ">")
        comparator = Comparator_GT;
    if (Operator == "<=")
        comparator = Comparator_LE;
    if (Operator == ">=")
        comparator = Comparator_GE;
    if (Operator == "==")
        comparator = Comparator_EQ;
    if (Operator == "!=")
        comparator = Comparator_NE;

    if (comparator)
        return builder.CreateComparison(comparator, left, right, inline_);

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
        return builder.CreateOperation(operator_, left, right, inline_);

    throw std::runtime_error("TODO");
}
