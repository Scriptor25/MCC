#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::SwitchStatement::SwitchStatement(
    SourceLocation where,
    ExpressionPtr condition,
    StatementPtr default_,
    std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases)
    : Statement(std::move(where)),
      Condition(std::move(condition)),
      Default(std::move(default_)),
      Cases(std::move(cases))
{
}

std::ostream &mcc::SwitchStatement::Print(std::ostream &stream) const
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

void mcc::SwitchStatement::Generate(Builder &builder) const
{
    const auto parent = builder.GetInsertParent();
    const auto start_target = builder.GetInsertBlock();
    const auto end_target = builder.CreateBlock(parent);
    const auto default_target = Default ? builder.CreateBlock(parent) : end_target;

    auto require_end = !Default;

    auto condition = Condition->GenerateValue(builder);

    if (Default)
    {
        builder.SetInsertBlock(default_target);
        Default->Generate(builder);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_end = true;
            (void) builder.CreateDirect(end_target);
        }
    }

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
        value_->Generate(builder);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_end = true;
            (void) builder.CreateDirect(end_target);
        }
    }

    builder.SetInsertBlock(start_target);
    (void) builder.CreateSwitch(std::move(condition), std::move(default_target), std::move(case_targets));

    if (!require_end)
    {
        builder.RemoveBlock(end_target);
        builder.SetInsertBlock(nullptr);
    }
    else
    {
        builder.SetInsertBlock(end_target);
    }
}
