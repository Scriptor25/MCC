#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::ThrowInstruction::Create(ResourceLocation location, ValuePtr value)
{
    return std::make_shared<ThrowInstruction>(std::move(location), std::move(value));
}

mcc::ThrowInstruction::ThrowInstruction(ResourceLocation location, ValuePtr value)
    : Location(std::move(location)),
      Value(std::move(value))
{
    Value->Use();
}

mcc::ThrowInstruction::~ThrowInstruction()
{
    Value->Drop();
}

void mcc::ThrowInstruction::Generate(CommandVector &commands, const bool stack) const
{
    switch (auto value = Value->GenerateResult(false); value.Type)
    {
        case ResultType_Value:
            commands.Append(
                "data modify storage {} result set value {}",
                Location,
                value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} result set from storage {} {}",
                Location,
                value.Location,
                value.Path);
            break;

        case ResultType_Score:
            commands.Append(
                "execute store result storage {} result double 1 run scoreboard players get {} {}",
                Location,
                value.Player,
                value.Objective);
            break;

        default:
            Error(
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                value.Type);
    }

    if (stack)
        commands.Append("data remove storage {} stack[0]", Location);

    commands.Append("return 1");
}

bool mcc::ThrowInstruction::RequireStack() const
{
    return Value->RequireStack();
}

bool mcc::ThrowInstruction::IsTerminator() const
{
    return true;
}
