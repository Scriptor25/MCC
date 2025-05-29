#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::SwitchExpression::SwitchExpression(const SourceLocation &where, ExpressionPtr condition, ExpressionPtr default_, std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases)
    : Expression(where), Condition(std::move(condition)), Default(std::move(default_)), Cases(std::move(cases))
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
    const auto start_target = builder.GetInsertBlock();

    const auto parent         = builder.GetInsertBlock()->Parent;
    const auto tail_target    = Block::Create(Where, builder.GetContext(), parent);
    const auto default_target = Block::Create(Default->Where, builder.GetContext(), parent);

    auto target_frame = frame;
    target_frame.Tail = tail_target;

    std::set<TypePtr> elements;
    std::vector<std::pair<BlockPtr, ValuePtr>> case_values;

    builder.SetInsertBlock(default_target);
    const auto default_value = Default->GenerateValue(builder, target_frame);
    elements.insert(default_value->Type);
    case_values.emplace_back(default_target, default_value);

    std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets;
    for (auto &[cases_, value_] : Cases)
    {
        auto case_target = Block::Create(value_->Where, builder.GetContext(), parent);

        for (auto &case_ : cases_)
        {
            auto value    = case_->GenerateValue(builder, frame);
            auto constant = std::dynamic_pointer_cast<Constant>(value);
            Assert(!!constant, case_->Where, "case entry must be constant");
            case_targets.emplace_back(constant, case_target);
        }

        builder.SetInsertBlock(case_target);
        const auto case_value = value_->GenerateValue(builder, target_frame);
        elements.insert(case_value->Type);
        case_values.emplace_back(case_target, case_value);
    }

    builder.SetInsertBlock(start_target);
    const auto condition = Condition->GenerateValue(builder, frame);
    (void) builder.CreateSwitch(Where, condition, default_target, case_targets);

    builder.SetInsertBlock(tail_target);
    const auto type          = (elements.size() == 1) ? *elements.begin() : builder.GetContext().GetUnion(elements);
    const auto branch_result = builder.CreateBranchResult(Where, type);

    for (auto &[target_, value_] : case_values)
    {
        builder.SetInsertBlock(target_);
        (void) builder.CreateDirect(value_->Where, tail_target, value_, branch_result);
    }

    builder.SetInsertBlock(tail_target);
    return branch_result;
}
