#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ObjectInstruction::CreateInsert(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key)
{
    return std::make_shared<ObjectInstruction>(
        where,
        location,
        object,
        value,
        key);
}

mcc::ObjectInstruction::ObjectInstruction(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key)
    : Instruction(where, TypeContext::GetVoid()),
      Location(location),
      Object(object),
      Value(value),
      Key(key)
{
    Object->Use();
    Value->Use();
}

mcc::ObjectInstruction::~ObjectInstruction()
{
    Object->Drop();
    Value->Drop();
}

void mcc::ObjectInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto object = Object->GenerateResult(false);
    auto value = Value->GenerateResult(false);

    switch (value.Type)
    {
        case ResultType_Value:
            commands.Append(
                "data modify storage {} {}.{} set value {}",
                object.Location,
                object.Path,
                Key,
                value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} {}.{} set from storage {} {}",
                object.Location,
                object.Path,
                Key,
                value.Location,
                value.Path);
            break;

        case ResultType_Score:
            commands.Append(
                "execute store result storage {} {}.{} double 1 run scoreboard players get {} {}",
                object.Location,
                object.Path,
                Key,
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
}

bool mcc::ObjectInstruction::RequireStack() const
{
    return Object->RequireStack() || Value->RequireStack();
}
