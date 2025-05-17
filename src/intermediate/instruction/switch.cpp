#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::SwitchInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &condition,
    const BlockPtr &default_target,
    const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets)
{
    return std::make_shared<SwitchInstruction>(
        where,
        location,
        condition,
        default_target,
        case_targets);
}

mcc::SwitchInstruction::SwitchInstruction(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &condition,
    const BlockPtr &default_target,
    const std::vector<std::pair<ConstantPtr, BlockPtr>> &case_targets)
    : Instruction(where, TypeID_Void),
      Location(location),
      Condition(condition),
      DefaultTarget(default_target),
      CaseTargets(case_targets)
{
    Condition->Use();
    DefaultTarget->Use();
    for (auto &[case_, target_]: CaseTargets)
    {
        case_->Use();
        target_->Use();
    }
}

mcc::SwitchInstruction::~SwitchInstruction()
{
    Condition->Drop();
    DefaultTarget->Drop();
    for (auto &[case_, target_]: CaseTargets)
    {
        case_->Drop();
        target_->Drop();
    }
}

void mcc::SwitchInstruction::Generate(CommandVector &commands, bool stack) const
{
    std::string prefix, arguments;
    (DefaultTarget ? DefaultTarget : CaseTargets.front().second)->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto tmp_name = GetTmpName();

    switch (auto condition = Condition->GenerateResult(false); condition.Type)
    {
        case ResultType_Value:
            for (auto &[case_, target_]: CaseTargets)
                if (auto case_value = case_->GenerateResult(false);
                    case_value.Value == condition.Value)
                    commands.Append(
                        "{}return run function {}{}",
                        prefix,
                        target_->Location,
                        arguments);
            break;

        case ResultType_Storage:
            for (auto &[case_, target_]: CaseTargets)
            {
                auto case_value = case_->GenerateResult(false);
                commands.Append(CreateTmpScore());
                commands.Append(
                    "execute store result score %c {} run data get storage {} {}",
                    tmp_name,
                    condition.Location,
                    condition.Path);
                commands.Append("data remove storage {} {}", Location, stack_path);
                commands.Append(
                    "execute if score %c {} matches {} run data modify storage {} {} set value 1",
                    tmp_name,
                    case_value.Value,
                    Location,
                    stack_path);
                commands.Append(RemoveTmpScore());
                commands.Append(
                    "{}execute if data storage {} {} run return run function {}{}",
                    prefix,
                    Location,
                    stack_path,
                    target_->Location,
                    arguments);
            }
            commands.Append("{}return run function {}{}", prefix, DefaultTarget->Location, arguments);
            break;

        case ResultType_Score:
            for (auto &[case_, target_]: CaseTargets)
            {
                auto case_value = case_->GenerateResult(false);
                commands.Append(
                    "{}execute if score {} {} matches {} run return run function {}{}",
                    prefix,
                    condition.Player,
                    condition.Objective,
                    case_value.Value,
                    target_->Location,
                    arguments);
            }
            commands.Append("{}return run function {}{}", prefix, DefaultTarget->Location, arguments);
            break;

        default:
            Error(
                Where,
                "condition must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                condition.Type);
    }
}

bool mcc::SwitchInstruction::RequireStack() const
{
    return Condition->RequireStack()
           || DefaultTarget->RequireStack()
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
