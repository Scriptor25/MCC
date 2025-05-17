#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::BranchInstruction::Create(
    ResourceLocation location,
    ValuePtr condition,
    BlockPtr then_target,
    BlockPtr else_target)
{
    return std::make_shared<BranchInstruction>(
        std::move(location),
        std::move(condition),
        std::move(then_target),
        std::move(else_target));
}

mcc::BranchInstruction::BranchInstruction(
    ResourceLocation location,
    ValuePtr condition,
    BlockPtr then_target,
    BlockPtr else_target)
    : Location(std::move(location)),
      Condition(std::move(condition)),
      ThenTarget(std::move(then_target)),
      ElseTarget(std::move(else_target))
{
    Condition->Use();
    ThenTarget->Use();
    ElseTarget->Use();
}

mcc::BranchInstruction::~BranchInstruction()
{
    Condition->Drop();
    ThenTarget->Drop();
    ElseTarget->Drop();
}

void mcc::BranchInstruction::Generate(CommandVector &commands, bool stack) const
{
    std::string prefix, arguments;
    ThenTarget->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto tmp_name = GetTmpName();

    auto then = ThenTarget->Location;
    auto else_ = ElseTarget->Location;

    switch (auto condition = Condition->GenerateResult(false); condition.Type)
    {
        case ResultType_Value:
            commands.Append(
                "{}return run function {}{}",
                prefix,
                condition.Value == "false" || condition.Value == "0"
                    ? else_
                    : then,
                arguments);
            break;

        case ResultType_Storage:
            commands.Append(CreateTmpScore());
            commands.Append(
                "execute store result score %c {} run data get storage {} {}",
                tmp_name,
                condition.Location,
                condition.Path);
            commands.Append(
                "data remove storage {} {}",
                Location,
                stack_path);
            commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
                tmp_name,
                Location,
                stack_path);
            commands.Append(RemoveTmpScore());

            commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                Location,
                stack_path,
                then,
                arguments);
            commands.Append(
                "{}return run function {}{}",
                prefix,
                else_,
                arguments);
            break;

        case ResultType_Score:
            commands.Append(
                "execute unless score {} {} matches 0 run return run function {}{}",
                condition.Player,
                condition.Objective,
                then,
                arguments);
            commands.Append(
                "{}return run function {}{}",
                prefix,
                else_,
                arguments);
            break;

        default:
            Error(
                "condition must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                condition.Type);
    }
}

bool mcc::BranchInstruction::RequireStack() const
{
    return true;
}

bool mcc::BranchInstruction::IsTerminator() const
{
    return true;
}
