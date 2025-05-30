#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::SwitchExpression::SwitchExpression(
    const SourceLocation &where,
    ExpressionPtr condition,
    ExpressionPtr default_,
    std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases)
    : Expression(where),
      Condition(std::move(condition)),
      Default(std::move(default_)),
      Cases(std::move(cases))
{
}

std::ostream &mcc::SwitchExpression::Print(std::ostream &stream) const
{
    Condition->Print(stream << "switch (") << ") {" << std::endl;

    for (auto &[conditions_, value_] : Cases)
    {
        stream << "case ";
        auto first = true;
        for (auto &condition : conditions_)
        {
            if (first)
                first = false;
            else
                stream << ", ";
            condition->Print(stream);
        }
        value_->Print(stream << " -> ") << std::endl;
    }

    if (Default)
        Default->Print(stream << "default -> ") << std::endl;

    return stream << '}';
}

mcc::ValuePtr mcc::SwitchExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    const auto condition = Condition->GenerateValue(builder, frame);
    Assert(
        condition->Type->IsNumber(),
        condition->Where,
        "condition must be of type number, but is {}",
        condition->Type);

    if (const auto constant_condition = std::dynamic_pointer_cast<ConstantNumber>(condition))
    {
        for (auto &[case_conditions_, case_] : Cases)
            for (auto &case_condition : case_conditions_)
            {
                auto case_condition_value = case_condition->GenerateValue(builder, frame);
                Assert(
                    case_condition_value->Type->IsNumber(),
                    case_condition->Where,
                    "case condition must be of type number, but is {}",
                    case_condition_value->Type);

                auto constant_case_condition = std::dynamic_pointer_cast<ConstantNumber>(case_condition_value);
                Assert(!!constant_case_condition, case_condition->Where, "case condition must be a constant number");

                if (constant_case_condition->Value == constant_condition->Value)
                    return case_->GenerateValue(builder, frame);
            }
        return Default->GenerateValue(builder, frame);
    }

    const auto pre_target = builder.GetInsertBlock();

    const auto parent = pre_target->Parent;
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);
    const auto default_target = Block::Create(Default->Where, builder.GetContext(), parent);

    std::set<TypePtr> elements;
    std::vector<std::pair<BlockPtr, ValuePtr>> case_values;

    builder.SetInsertBlock(default_target);
    const auto default_value = Default->GenerateValue(builder, frame);
    elements.insert(default_value->Type);
    case_values.emplace_back(default_target, default_value);

    std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets;
    for (auto &[case_conditions_, case_] : Cases)
    {
        auto case_target = Block::Create(case_->Where, builder.GetContext(), parent);

        for (auto &case_condition : case_conditions_)
        {
            auto case_condition_value = case_condition->GenerateValue(builder, frame);
            Assert(
                case_condition_value->Type->IsNumber(),
                case_condition->Where,
                "case condition must be of type number, but is {}",
                case_condition_value->Type);

            auto constant_case_condition = std::dynamic_pointer_cast<ConstantNumber>(case_condition_value);
            Assert(!!constant_case_condition, case_condition->Where, "case condition must be a constant number");

            case_targets.emplace_back(constant_case_condition, case_target);
        }

        builder.SetInsertBlock(case_target);
        const auto case_value = case_->GenerateValue(builder, frame);
        elements.insert(case_value->Type);
        case_values.emplace_back(case_target, case_value);
    }

    builder.SetInsertBlock(pre_target);
    (void) builder.CreateSwitch(Where, condition, default_target, case_targets);

    builder.SetInsertBlock(tail_target);
    const auto type = elements.size() == 1
                          ? *elements.begin()
                          : builder.GetContext().GetUnion(elements);
    const auto branch_result = builder.CreateBranchResult(Where, type);

    for (auto &[target_, value_] : case_values)
    {
        builder.SetInsertBlock(target_);
        (void) builder.CreateDirect(value_->Where, tail_target, value_, branch_result);
    }

    builder.SetInsertBlock(tail_target);
    return branch_result;
}
