#include <utility>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::SwitchInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &condition,
    const BlockPtr &default_target,
    const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets)
{
    return std::make_shared<SwitchInstruction>(where, context, location, condition, default_target, case_targets);
}

mcc::SwitchInstruction::SwitchInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr condition,
    BlockPtr default_target,
    const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets)
    : Instruction(where, context.GetVoid(), false),
      Location(std::move(location)),
      Condition(std::move(condition)),
      DefaultTarget(std::move(default_target)),
      CaseTargets(case_targets)
{
    Condition->Use();
    DefaultTarget->Use();
    for (auto &[case_, target_] : CaseTargets)
    {
        case_->Use();
        target_->Use();
    }
}

mcc::SwitchInstruction::~SwitchInstruction()
{
    Condition->Drop();
    DefaultTarget->Drop();
    for (auto &[case_, target_] : CaseTargets)
    {
        case_->Drop();
        target_->Drop();
    }
}

void mcc::SwitchInstruction::Generate(CommandVector &commands, bool stack) const
{
    std::string prefix, arguments;
    (DefaultTarget ? DefaultTarget : CaseTargets.front().second)->Parent->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto tmp_name = GetTemp();

    auto take_default = true;

    auto condition = Condition->GenerateResult();

    std::string condition_prefix;
    if (condition.WithArgument)
    {
        condition_prefix = "$";
    }

    switch (condition.Type)
    {
    case ResultType_Value:
        for (auto &[case_, target_] : CaseTargets)
        {
            auto case_value = case_->GenerateResult();
            Assert(
                case_value.Type == ResultType_Value,
                case_->Where,
                "case value must be {}, but is {}",
                ResultType_Value,
                case_value.Type);

            if (case_value.Value != condition.Value)
                continue;

            commands.Append("{}return run function {}{}", prefix, target_->GetLocation(), arguments);
            take_default = false;
            break;
        }
        if (take_default)
            commands.Append(
                "{}return run function {}{}",
                prefix,
                DefaultTarget->GetLocation(),
                arguments);
        break;

    case ResultType_Reference:
        for (auto &[case_, target_] : CaseTargets)
        {
            auto case_value = case_->GenerateResult();
            Assert(
                case_value.Type == ResultType_Value,
                case_->Where,
                "case value must be {}, but is {}",
                ResultType_Value,
                case_value.Type);

            commands.Append(CreateScore());
            commands.Append(
                "{}execute store result score %c {} run data get {} {} {}",
                condition_prefix,
                tmp_name,
                condition.ReferenceType,
                condition.Target,
                condition.Path);
            commands.Append("data remove storage {} {}", Location, stack_path);
            commands.Append(
                "execute if score %c {} matches {} run data modify storage {} {} set value 1",
                tmp_name,
                case_value.Value,
                Location,
                stack_path);
            commands.Append(RemoveScore());

            commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                Location,
                stack_path,
                target_->GetLocation(),
                arguments);
        }
        commands.Append(
            "{}return run function {}{}",
            prefix,
            DefaultTarget->GetLocation(),
            arguments);
        break;

    case ResultType_Argument:
        for (auto &[case_, target_] : CaseTargets)
        {
            auto case_value = case_->GenerateResult();

            commands.Append(CreateScore());
            commands.Append("$scoreboard players set %c {} {}", tmp_name, condition.Name);
            commands.Append("data remove storage {} {}", Location, stack_path);
            commands.Append(
                "execute if score %c {} matches {} run data modify storage {} {} set value 1",
                tmp_name,
                case_value.Value,
                Location,
                stack_path);
            commands.Append(RemoveScore());

            commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                Location,
                stack_path,
                target_->GetLocation(),
                arguments);
        }
        commands.Append(
            "{}return run function {}{}",
            prefix,
            DefaultTarget->GetLocation(),
            arguments);
        break;

    default:
        Error(
            Where,
            "condition must be {}, {} or {}, but is {}",
            ResultType_Value,
            ResultType_Reference,
            ResultType_Argument,
            condition.Type);
    }
}

bool mcc::SwitchInstruction::RequireStack() const
{
    return Condition->RequireStack() || DefaultTarget->RequireStack()
           || std::ranges::any_of(
               CaseTargets,
               [](auto &case_target)
               {
                   return case_target.second->RequireStack();
               });
}

bool mcc::SwitchInstruction::IsTerminator() const
{
    return true;
}
