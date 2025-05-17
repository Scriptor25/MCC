#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
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

    for (auto &[conditions_, value_]: Cases)
    {
        stream << "case ";
        auto first = true;
        for (auto &condition: conditions_)
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

    const auto parent = builder.GetInsertParent(Where);
    const auto tail_target = builder.CreateBlock(Where, parent);
    const auto default_target = builder.CreateBlock(Default->Where, parent);

    auto target_frame = frame;
    target_frame.Tail = tail_target;

    builder.SetInsertBlock(tail_target);
    const auto branch_result = builder.CreateBranchResult(Where, TypeID_Any);

    builder.SetInsertBlock(default_target);
    const auto default_value = Default->GenerateValue(builder, target_frame);
    (void) builder.CreateDirect(Default->Where, tail_target, default_value, branch_result);

    std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets;
    for (auto &[cases_, value_]: Cases)
    {
        auto case_target = builder.CreateBlock(value_->Where, parent);
        builder.SetInsertBlock(case_target);
        for (auto &case_: cases_)
        {
            auto value = case_->GenerateValue(builder, frame);
            auto constant = std::dynamic_pointer_cast<Constant>(value);
            Assert(!!constant, case_->Where, "case entry must be constant");
            case_targets.emplace_back(constant, case_target);
        }
        auto case_value = value_->GenerateValue(builder, target_frame);
        (void) builder.CreateDirect(value_->Where, tail_target, case_value, branch_result);
    }

    builder.SetInsertBlock(start_target);
    const auto condition = Condition->GenerateValue(builder, frame);
    (void) builder.CreateSwitch(Where, condition, default_target, case_targets);

    builder.SetInsertBlock(tail_target);
    return branch_result;
}
