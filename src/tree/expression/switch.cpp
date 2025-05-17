#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::SwitchExpression::SwitchExpression(
    SourceLocation where,
    ExpressionPtr condition,
    ExpressionPtr default_,
    std::vector<std::pair<std::vector<ExpressionPtr>, ExpressionPtr>> cases)
    : Expression(std::move(where)),
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

mcc::ValuePtr mcc::SwitchExpression::GenerateValue(Builder &builder) const
{
    const auto parent = builder.GetInsertParent();
    const auto start_target = builder.GetInsertBlock();
    const auto end_target = builder.CreateBlock(parent);
    const auto default_target = builder.CreateBlock(parent);

    auto condition = Condition->GenerateValue(builder);

    builder.SetInsertBlock(end_target);
    const auto branch_result = builder.CreateBranchResult();

    builder.SetInsertBlock(default_target);
    auto default_value = Default->GenerateValue(builder);
    (void) builder.CreateDirect(end_target, std::move(default_value), branch_result);

    std::vector<std::pair<ConstantPtr, BlockPtr>> case_targets;
    for (auto &[cases_, value_]: Cases)
    {
        auto case_target = builder.CreateBlock(parent);
        builder.SetInsertBlock(case_target);
        for (auto &case_: cases_)
        {
            auto value = case_->GenerateValue(builder);
            auto constant = std::dynamic_pointer_cast<Constant>(value);
            Assert(!!constant, "case entry must be constant");
            case_targets.emplace_back(constant, case_target);
        }
        auto case_value = value_->GenerateValue(builder);
        (void) builder.CreateDirect(end_target, std::move(case_value), branch_result);
    }

    builder.SetInsertBlock(start_target);
    (void) builder.CreateSwitch(std::move(condition), std::move(default_target), std::move(case_targets));

    builder.SetInsertBlock(end_target);
    return branch_result;
}
