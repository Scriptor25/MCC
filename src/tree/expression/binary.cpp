#include <mcc/builder.hpp>
#include <mcc/common.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

const std::string &mcc::BinaryExpression::MapOperator(const std::string &operator_)
{
    static const std::map<std::string, std::string> operator_map = {
        {  "+",                "__add" },
        {  "-",                "__sub" },
        {  "*",                "__mul" },
        {  "/",                "__div" },
        {  "%",                "__rem" },

        {  "=",                "__set" },

        { "+=",            "__add_set" },
        { "-=",            "__sub_set" },
        { "*=",            "__mul_set" },
        { "/=",            "__div_set" },
        { "%=",            "__rem_set" },

        { "==",              "__equal" },
        {  "<",          "__less_than" },
        {  ">",       "__greater_than" },
        { "<=",    "__less_than_equal" },
        { ">=", "__greater_than_equal" },
    };

    return operator_map.at(operator_);
}

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
    static const std::set<std::string_view> mergeable = { "+", "-", "*", "/", "%" };

    if (!mergeable.contains(Operator))
        return nullptr;

    auto merged = false;
    std::vector<ExpressionPtr> operands;

    if (const auto left = dynamic_cast<BinaryExpression *>(Left.get()); left && left->Operator == Operator)
    {
        merged = true;
        operands.push_back(std::move(left->Left));
        operands.push_back(std::move(left->Right));
    }
    else
        operands.push_back(std::move(Left));

    if (const auto right = dynamic_cast<BinaryExpression *>(Right.get()); right && right->Operator == Operator)
    {
        merged = true;
        operands.push_back(std::move(right->Left));
        operands.push_back(std::move(right->Right));
    }
    else
        operands.push_back(std::move(Right));

    if (!merged)
    {
        Left  = std::move(operands.front());
        Right = std::move(operands.back());
        return nullptr;
    }

    return std::make_unique<VectorExpression>(Where, Operator, std::move(operands));
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return Right->Print(Left->Print(stream) << ' ' << Operator << ' ');
}

mcc::ValuePtr mcc::BinaryExpression::GenerateValue(
        Builder &builder,
        const Frame &frame) const
{
    auto left  = Left->GenerateValue(builder, frame);
    auto right = Right->GenerateValue(builder, frame);

    const ParameterRefList parameters = {
        {  left->Type,  left->FieldType },
        { right->Type, right->FieldType },
    };

    if (const auto candidates = builder.FindFunctions({ "operator", MapOperator(Operator) }, parameters, false);
        !candidates.empty())
    {
        const auto callee = builder.FindUnambiguousCandidate(Where, candidates, parameters);

        return builder.CreateCall(Where, {}, callee, { left, right }, frame.LandingPad);
    }

    if (const auto constant_left = std::dynamic_pointer_cast<ConstantNumber>(left),
        constant_right           = std::dynamic_pointer_cast<ConstantNumber>(right);
        constant_left && constant_right)
    {
        if (const auto operator_ = ToOperator(Operator))
            switch (*operator_)
            {
            case Operator_::Add:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value + constant_right->Value);
            case Operator_::Sub:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value - constant_right->Value);
            case Operator_::Mul:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value * constant_right->Value);
            case Operator_::Div:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value / constant_right->Value);
            case Operator_::Rem:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value % constant_right->Value);
            }

        if (const auto comparator = ToComparator(Operator))
            switch (*comparator)
            {
            case Comparator_::EQ:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value == constant_right->Value);
            case Comparator_::LT:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value < constant_right->Value);
            case Comparator_::GT:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value > constant_right->Value);
            case Comparator_::LE:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value <= constant_right->Value);
            case Comparator_::GE:
                return ConstantNumber::Create(
                        Where,
                        builder.GetContext(),
                        constant_left->Value >= constant_right->Value);
            }
    }

    if (Operator == "=")
        return builder.CreateStore(Where, {}, left, right);

    Assert(left->Type->IsNumber(), Left->Where, "left must be of type number, but is {}", left->Type);
    Assert(right->Type->IsNumber(), Right->Where, "right must be of type number, but is {}", right->Type);

    if (const auto comparator = ToComparator(Operator))
        return builder.CreateComparison(Where, {}, *comparator, left, right);

    const auto store     = Operator.back() == '=';
    auto operator_string = Operator;
    if (store)
        operator_string.pop_back();

    if (const auto operator_ = ToOperator(operator_string))
    {
        auto operation = builder.CreateOperation(Where, {}, *operator_, { left, right });
        if (store)
            return builder.CreateStore(Where, {}, left, operation);
        return operation;
    }

    Error(Where, "undefined binary operator {}", Operator);
}
