#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::ObjectInstruction::CreateInsert(
    ResourceLocation location,
    ValuePtr object,
    ValuePtr value,
    std::string key)
{
    return std::make_shared<ObjectInstruction>(
        std::move(location),
        std::move(object),
        std::move(value),
        std::move(key));
}

mcc::ObjectInstruction::ObjectInstruction(ResourceLocation location, ValuePtr object, ValuePtr value, std::string key)
    : Location(std::move(location)),
      Object(std::move(object)),
      Value(std::move(value)),
      Key(std::move(key))
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
    //
    // object: stack-allocated
    //
    //      data modify storage <location> stack[0].values[<object.index>].<key> set ...
    //
    // value -> constant:           ... value <value>
    //       -> stack-allocated:    ... from storage <location> stack[0].values[<value.index>]
    //
    // value -> register:
    //
    //      execute store result storage <location> stack[0].values[<object.index>].<key> double 1 run scoreboard players get <value.player> <value.objective>
    //

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
