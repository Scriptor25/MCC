#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

#include <algorithm>
#include <ranges>
#include <utility>

mcc::InstructionPtr mcc::SwitchInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const ValuePtr &condition,
        const BlockPtr &default_target,
        const CaseTargetMap &case_targets)
{
    auto self =
            std::make_shared<SwitchInstruction>(where, name, context, parent, condition, default_target, case_targets);

    self->Self = self;
    self->Condition->Use(self);
    self->DefaultTarget->Use(self);
    for (auto &[case_, target_] : self->CaseTargets)
    {
        case_->Use(self);
        target_->Use(self);
    }

    return self;
}

mcc::SwitchInstruction::SwitchInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent,
        ValuePtr condition,
        BlockPtr default_target,
        CaseTargetMap case_targets)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Parent(std::move(parent)),
      Condition(std::move(condition)),
      DefaultTarget(std::move(default_target)),
      CaseTargets(std::move(case_targets))
{
}

mcc::SwitchInstruction::~SwitchInstruction()
{
    Condition->Drop(Self);
    DefaultTarget->Drop(Self);
    for (auto &[case_, target_] : CaseTargets)
    {
        case_->Drop(Self);
        target_->Drop(Self);
    }
}

void mcc::SwitchInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto location = Parent->Mangle();

    std::string prefix, arguments;
    Parent->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto tmp_name   = GetTemp();

    auto condition = Condition->GenerateResult();

    std::string condition_prefix;
    if (condition.WithArgument)
        condition_prefix = "$";

    std::map<std::string, BlockPtr> case_targets;
    for (auto &[case_, target_] : CaseTargets)
    {
        auto case_value = case_->GenerateResult();
        Assert(case_value.Type == ResultType_::Value,
               case_->Where,
               "case value must be {}, but is {}",
               ResultType_::Value,
               case_value.Type);
        case_targets[case_value.Value] = target_;
    }

    switch (condition.Type)
    {
    case ResultType_::Value:
    {
        BlockPtr block;
        if (auto it = case_targets.find(condition.Value); it != case_targets.end())
            block = it->second;
        else
            block = DefaultTarget;
        commands.Append("{}return run function {}{}", prefix, block->GetLocation(), arguments);
        break;
    }

    case ResultType_::Reference:
        for (auto &[case_, target_] : case_targets)
        {
            commands.Append(CreateScore());
            commands.Append(
                    "{}execute store result score %c {} run data get {} {} {}",
                    condition_prefix,
                    tmp_name,
                    condition.ReferenceType,
                    condition.Target,
                    condition.Path);
            commands.Append("data remove storage {} {}", location, stack_path);
            commands.Append(
                    "execute if score %c {} matches {} run data modify storage {} {} set value 1",
                    tmp_name,
                    case_,
                    location,
                    stack_path);
            commands.Append(RemoveScore());

            commands.Append(
                    "{}execute if data storage {} {} run return run function {}{}",
                    prefix,
                    location,
                    stack_path,
                    target_->GetLocation(),
                    arguments);
        }
        commands.Append("{}return run function {}{}", prefix, DefaultTarget->GetLocation(), arguments);
        break;

    case ResultType_::Argument:
        for (auto &[case_, target_] : case_targets)
        {
            commands.Append(CreateScore());
            commands.Append("$scoreboard players set %c {} {}", tmp_name, condition.Name);
            commands.Append("data remove storage {} {}", location, stack_path);
            commands.Append(
                    "execute if score %c {} matches {} run data modify storage {} {} set value 1",
                    tmp_name,
                    case_,
                    location,
                    stack_path);
            commands.Append(RemoveScore());

            commands.Append(
                    "{}execute if data storage {} {} run return run function {}{}",
                    prefix,
                    location,
                    stack_path,
                    target_->GetLocation(),
                    arguments);
        }
        commands.Append("{}return run function {}{}", prefix, DefaultTarget->GetLocation(), arguments);
        break;

    default:
        Error(Where,
              "condition must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
              condition.Type);
    }
}

bool mcc::SwitchInstruction::RequireStack() const
{
    return Condition->RequireStack() || DefaultTarget->RequireStack()
           || std::ranges::any_of(
                   CaseTargets,
                   [](const CaseTarget &case_target) { return case_target.second->RequireStack(); });
}

bool mcc::SwitchInstruction::IsTerminator() const
{
    return true;
}

void mcc::SwitchInstruction::Replace(
        const ValuePtr value,
        const ValuePtr replacement)
{
    Assert(!!value, "value must not be null");
    Assert(!!replacement, "replacement must not be null");

    if (value == Condition)
    {
        Condition->Drop(Self);
        Condition = replacement;
        Condition->Use(Self);
        return;
    }

    if (value == DefaultTarget)
    {
        DefaultTarget->Drop(Self);
        DefaultTarget = std::dynamic_pointer_cast<Block>(replacement);
        DefaultTarget->Use(Self);
        return;
    }

    for (auto &target : CaseTargets | std::views::values)
    {
        if (value == target)
        {
            target->Drop(Self);
            target = std::dynamic_pointer_cast<Block>(replacement);
            target->Use(Self);
            return;
        }
    }

    Error("value is not owned by this instruction");
}
