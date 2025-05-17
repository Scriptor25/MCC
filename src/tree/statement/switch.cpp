#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/value.hpp>

mcc::SwitchStatement::SwitchStatement(
    const SourceLocation &where,
    ExpressionPtr condition,
    StatementPtr default_,
    std::vector<std::pair<std::vector<ExpressionPtr>, StatementPtr>> cases)
    : Statement(where),
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

void mcc::SwitchStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto start_target = builder.GetInsertBlock();

    const auto parent = builder.GetInsertParent(Where);
    const auto tail_target = builder.CreateBlock(Where, parent);
    const auto default_target = Default ? builder.CreateBlock(Default->Where, parent) : tail_target;

    auto require_tail = !Default;

    auto target_frame = frame;
    target_frame.Tail = tail_target;

    if (Default)
    {
        builder.SetInsertBlock(default_target);
        Default->Generate(builder, target_frame);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(Default->Where, tail_target);
        }
    }

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
        value_->Generate(builder, target_frame);
        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(value_->Where, tail_target);
        }
    }

    builder.SetInsertBlock(start_target);
    const auto condition = Condition->GenerateValue(builder, frame);
    (void) builder.CreateSwitch(
        Condition->Where,
        condition,
        default_target,
        case_targets);

    if (!require_tail && !(target_frame.Flags & FrameFlag_RequireTail))
    {
        builder.RemoveBlock(Where, tail_target);
        builder.SetInsertBlock(nullptr);
    }
    else
    {
        builder.SetInsertBlock(tail_target);
    }
}
