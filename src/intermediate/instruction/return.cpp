#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(ResourceLocation location, ValuePtr value)
{
    return std::make_shared<ReturnInstruction>(std::move(location), std::move(value));
}

mcc::ReturnInstruction::ReturnInstruction(ResourceLocation location, ValuePtr value)
    : Location(std::move(location)),
      Value(std::move(value))
{
    if (Value)
        Value->Use();
}

mcc::ReturnInstruction::~ReturnInstruction()
{
    if (Value)
        Value->Drop();
}

void mcc::ReturnInstruction::Generate(CommandVector &commands, bool stack) const
{
    if (!Value)
    {
        if (stack)
            commands.Append("data remove storage {} stack[0]", Location);
        commands.Append("return 0");
        return;
    }

    auto value = Value->GenerateResult(false);

    switch (value.Type)
    {
        case ResultType_Value:
            if (stack)
                commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return {}", value.Value);
            break;

        case ResultType_Storage:
            if (stack)
            {
                commands.Append(
                    "data modify storage {} result set from storage {} {}",
                    Location,
                    value.Location,
                    value.Path);
                commands.Append("data remove storage {} stack[0]", Location);
                commands.Append("return run data get storage {} result", Location);
                break;
            }
            commands.Append("return run data get storage {} {}", value.Location, value.Path);
            break;

        case ResultType_Score:
            if (stack)
                commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return run scoreboard players get {} {}", value.Player, value.Objective);
            break;

        default:
            Error(
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                value.Type);
    }
}

bool mcc::ReturnInstruction::RequireStack() const
{
    return Value ? Value->RequireStack() : false;
}

bool mcc::ReturnInstruction::IsTerminator() const
{
    return true;
}
