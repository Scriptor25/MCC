#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &value)
{
    return std::make_shared<ReturnInstruction>(where, location, value);
}

mcc::ReturnInstruction::ReturnInstruction(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &value)
    : Instruction(where, TypeContext::GetVoid()),
      Location(location),
      Value(value)
{
    if (Value)
        Value->Use();
}

mcc::ReturnInstruction::~ReturnInstruction()
{
    if (Value)
        Value->Drop();
}

void mcc::ReturnInstruction::Generate(CommandVector &commands, const bool stack) const
{
    if (Value)
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
                    Where,
                    "value must be {}, {} or {}, but is {}",
                    ResultType_Value,
                    ResultType_Storage,
                    ResultType_Score,
                    value.Type);
        }
    else
        commands.Append("data remove storage {} result", Location);

    if (stack)
        commands.Append("data remove storage {} stack[0]", Location);

    commands.Append("return 0");
}

bool mcc::ReturnInstruction::RequireStack() const
{
    return Value ? Value->RequireStack() : false;
}

bool mcc::ReturnInstruction::IsTerminator() const
{
    return true;
}
