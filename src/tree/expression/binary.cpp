#include <set>
#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::BinaryExpression::BinaryExpression(
    const SourceLocation &where,
    std::string operator_,
    ExpressionPtr left,
    ExpressionPtr right)
    : Expression(where),
      Operator(std::move(operator_)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

mcc::ExpressionPtr mcc::BinaryExpression::Merge()
{
    static const std::set<std::string_view> mergeable
    {
        "+",
        "-",
        "*",
        "/",
        "%",
    };

    if (!mergeable.contains(Operator))
        return nullptr;

    auto merged = false;
    std::vector<ExpressionPtr> operands;

    if (const auto left = dynamic_cast<BinaryExpression *>(Left.get()); left && left->Operator == Operator)
    {
        merged = true;
        operands.emplace_back(std::move(left->Left));
        operands.emplace_back(std::move(left->Right));
    }
    else
        operands.emplace_back(std::move(Left));

    if (const auto right = dynamic_cast<BinaryExpression *>(Right.get()); right && right->Operator == Operator)
    {
        merged = true;
        operands.emplace_back(std::move(right->Left));
        operands.emplace_back(std::move(right->Right));
    }
    else
        operands.emplace_back(std::move(Right));

    if (!merged)
    {
        Left = std::move(operands.front());
        Right = std::move(operands.back());
        return nullptr;
    }

    return std::make_unique<VectorExpression>(Where, Operator, std::move(operands));
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return Right->Print(Left->Print(stream) << ' ' << Operator << ' ');
}

mcc::ValuePtr mcc::BinaryExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    auto left = Left->GenerateValue(builder, frame);
    auto right = Right->GenerateValue(builder, frame);

    const auto constant_left = std::dynamic_pointer_cast<ConstantNumber>(left);
    const auto constant_right = std::dynamic_pointer_cast<ConstantNumber>(right);

    if (constant_left && constant_right)
    {
        if (Operator == "<")
            return ConstantBoolean::Create(
                Where,
                builder.GetContext(),
                constant_left->Value < constant_right->Value);
        if (Operator == ">")
            return ConstantBoolean::Create(
                Where,
                builder.GetContext(),
                constant_left->Value > constant_right->Value);
        if (Operator == "<=")
            return ConstantBoolean::Create(
                Where,
                builder.GetContext(),
                constant_left->Value <= constant_right->Value);
        if (Operator == ">=")
            return ConstantBoolean::Create(
                Where,
                builder.GetContext(),
                constant_left->Value >= constant_right->Value);
        if (Operator == "==")
            return ConstantBoolean::Create(
                Where,
                builder.GetContext(),
                constant_left->Value == constant_right->Value);

        if (Operator == "+")
            return ConstantNumber::Create(
                Where,
                builder.GetContext(),
                constant_left->Value + constant_right->Value);
        if (Operator == "-")
            return ConstantNumber::Create(
                Where,
                builder.GetContext(),
                constant_left->Value - constant_right->Value);
        if (Operator == "*")
            return ConstantNumber::Create(
                Where,
                builder.GetContext(),
                constant_left->Value * constant_right->Value);
        if (Operator == "/")
            return ConstantNumber::Create(
                Where,
                builder.GetContext(),
                constant_left->Value / constant_right->Value);
        if (Operator == "%")
            return ConstantNumber::Create(
                Where,
                builder.GetContext(),
                constant_left->Value % constant_right->Value);
    }

    if (Operator == "=")
        return builder.CreateStore(Where, left, right);

    Assert(left->Type->IsNumber(), Left->Where, "left must be of type number, but is {}", left->Type);
    Assert(right->Type->IsNumber(), Right->Where, "right must be of type number, but is {}", right->Type);

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

    if (comparator)
        return builder.CreateComparison(Where, comparator, left, right);

    const auto store = Operator.back() == '=';
    auto operator_string = Operator;
    if (store)
        operator_string.pop_back();

    if (const auto operator_ = ToOperator(operator_string))
    {
        auto operation = builder.CreateOperation(Where, *operator_, { left, right });
        if (store)
            return builder.CreateStore(Where, left, operation);
        return operation;
    }

    Error(Where, "undefined binary operator {}", Operator);
}
