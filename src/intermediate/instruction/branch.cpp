#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

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

void mcc::BranchInstruction::Generate(CommandVector &commands) const
{
    auto condition = Condition->GenerateResult(false);

    auto then = ThenTarget->Location;
    auto else_ = ElseTarget->Location;

    switch (condition.Type)
    {
        case ResultType_Value:
            commands.Append(
                "return run function {}",
                condition.Value == "false" || condition.Value == "0"
                    ? else_
                    : then);
            break;

        case ResultType_Storage:
            commands.Append(CreateTmpScore());
            commands.Append(
                "execute store result score %c {} run data get storage {} {}",
                GetTmpName(),
                condition.Location,
                condition.Path);
            commands.Append("data remove storage {} result", Location);
            commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} result set value 1",
                GetTmpName(),
                Location);
            commands.Append(RemoveTmpScore());
            commands.Append("execute if data storage {} result run return run function {}", Location, then);
            commands.Append("return run function {}", else_);
            break;

        case ResultType_Score:
            commands.Append(
                "execute unless score {} {} matches 0 run return run function {}",
                condition.Player,
                condition.Objective,
                then);
            commands.Append("return run function {}", else_);
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

bool mcc::BranchInstruction::IsTerminator() const
{
    return true;
}

mcc::InstructionPtr mcc::DirectInstruction::Create(BlockPtr target)
{
    return std::make_shared<DirectInstruction>(std::move(target));
}

mcc::DirectInstruction::DirectInstruction(BlockPtr target)
    : Target(std::move(target))
{
}

void mcc::DirectInstruction::Generate(CommandVector &commands) const
{
    auto target = Target->Location;
    commands.Append("return run function {}", target);
}

bool mcc::DirectInstruction::IsTerminator() const
{
    return true;
}
