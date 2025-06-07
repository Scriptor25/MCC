#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
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

void mcc::SwitchStatement::Generate(Builder &builder, Frame &frame) const
{
    const auto condition = Condition->GenerateValue(builder, frame);
    Assert(
        condition->Type->IsNumber(),
        Condition->Where,
        "condition must be of type number, but is {}",
        condition->Type);

    const auto pre_target = builder.GetInsertBlock();
    const auto parent = pre_target->Parent;
    const auto tail_target = Block::Create(Where, builder.GetContext(), parent);

    auto require_tail = !Default;

    auto target_frame = frame;
    target_frame.Tail = tail_target;

    if (const auto constant_condition = std::dynamic_pointer_cast<ConstantNumber>(condition))
    {
        auto take_default = true;

        for (auto &[case_conditions_, case_] : Cases)
        {
            if (!take_default)
                break;

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
                {
                    take_default = false;
                    case_->Generate(builder, target_frame);
                    if (!builder.GetInsertBlock()->GetTerminator())
                    {
                        require_tail = true;
                        (void) builder.CreateDirect(case_->Where, tail_target);
                        break;
                    }
                }
            }
        }

        if (take_default && Default)
        {
            Default->Generate(builder, target_frame);
            if (!builder.GetInsertBlock()->GetTerminator())
            {
                require_tail = true;
                (void) builder.CreateDirect(Default->Where, tail_target);
            }
        }

        if (!require_tail && !(target_frame.Flags & FrameFlag_RequireTail))
        {
            tail_target->Erase();
            builder.SetInsertBlock(nullptr);
        }
        else
            builder.SetInsertBlock(tail_target);
        return;
    }

    const auto default_target = Default ? Block::Create(Default->Where, builder.GetContext(), parent) : tail_target;

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
        case_->Generate(builder, target_frame);

        if (!builder.GetInsertBlock()->GetTerminator())
        {
            require_tail = true;
            (void) builder.CreateDirect(case_->Where, tail_target);
        }
    }

    builder.SetInsertBlock(pre_target);
    (void) builder.CreateSwitch(Condition->Where, condition, default_target, case_targets);

    if (!require_tail && !(target_frame.Flags & FrameFlag_RequireTail))
    {
        tail_target->Erase();
        builder.SetInsertBlock(nullptr);
    }
    else
        builder.SetInsertBlock(tail_target);
}
