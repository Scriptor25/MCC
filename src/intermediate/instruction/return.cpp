#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(ResourceLocation location, ValuePtr value)
{
    return std::make_shared<ReturnInstruction>(std::move(location), std::move(value));
}

mcc::ReturnInstruction::ReturnInstruction(ResourceLocation location, ValuePtr value)
    : Location(std::move(location)),
      Value(std::move(value))
{
    Value->Use();
}

mcc::ReturnInstruction::~ReturnInstruction()
{
    Value->Drop();
}

void mcc::ReturnInstruction::Generate(const Builder &builder, CommandVector &commands, const bool use_stack) const
{
    auto value = Value->GenerateResult(false, use_stack);

    switch (value.Type)
    {
        case ResultType_Value:
            if (use_stack)
                commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return {}", value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} result set from storage {} {}",
                Location,
                value.Location,
                value.Path);
            if (use_stack)
                commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return run data get storage {} result", Location);
            break;

        case ResultType_Score:
            if (use_stack)
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
    return Value->RequireStack();
}
