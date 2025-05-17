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
    auto tmp = GetTmpName();

    auto condition = Condition->GenerateResult(false);

    auto then = ThenTarget->Location;
    auto else_ = ElseTarget->Location;

    std::string arguments;
    std::string prefix;

    if (auto &parameters = ThenTarget->Parent->Parameters; !parameters.empty())
    {
        prefix = "$";
        arguments += " {";
        for (unsigned i = 0; i < parameters.size(); ++i)
        {
            if (i)
                arguments += ',';
            arguments += std::format("{0}:$({0})", parameters[i]);
        }
        arguments += '}';
    }

    switch (condition.Type)
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
                tmp,
                condition.Location,
                condition.Path);
            commands.Append("data remove storage {} result", Location);
            commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} result set value 1",
                tmp,
                Location);
            commands.Append(RemoveTmpScore());
            commands.Append(
                "{}execute if data storage {} result run return run function {}{}",
                prefix,
                Location,
                then,
                arguments);
            commands.Append("{}return run function {}{}", prefix, else_, arguments);
            break;

        case ResultType_Score:
            commands.Append(
                "execute unless score {} {} matches 0 run return run function {}{}",
                condition.Player,
                condition.Objective,
                then,
                arguments);
            commands.Append("{}return run function {}{}", prefix, else_, arguments);
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
    return Condition->RequireStack();
}

bool mcc::BranchInstruction::IsTerminator() const
{
    return true;
}
