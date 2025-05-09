#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::BinaryExpression::BinaryExpression(
    SourceLocation where,
    std::string operator_,
    ExpressionPtr left,
    ExpressionPtr right)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return Right->Print(Left->Print(stream) << ' ' << Operator << ' ');
}

mcc::ValuePtr mcc::BinaryExpression::GenerateValue(Builder &builder) const
{
    auto left = Left->GenerateValue(builder);
    auto right = Right->GenerateValue(builder);

    if (Operator == "=")
        return builder.CreateStore(std::move(left), std::move(right));

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
        return builder.CreateComparison(comparator, std::move(left), std::move(right));

    const auto store = Operator.back() == '=';
    auto operator_string = Operator;
    if (store)
        operator_string.pop_back();

    auto operator_ = Operator_None;
    if (operator_string == "+")
        operator_ = Operator_Add;
    if (operator_string == "-")
        operator_ = Operator_Sub;
    if (operator_string == "*")
        operator_ = Operator_Mul;
    if (operator_string == "/")
        operator_ = Operator_Div;
    if (operator_string == "%")
        operator_ = Operator_Rem;

    if (operator_)
    {
        auto operation = builder.CreateOperation(operator_, left, std::move(right));
        if (store)
            return builder.CreateStore(std::move(left), std::move(operation));
        return std::move(operation);
    }

    Error("undefined binary operator {}", Operator);
}
