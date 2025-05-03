#include <mcc/intermediate.hpp>

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
}

void mcc::ObjectInstruction::Gen(CommandVector &commands) const
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

    auto [
        object_type_,
        object_value_,
        object_path_,
        object_player,
        object_objective_
    ] = Object->GenResult();
    auto [
        value_type_,
        value_value_,
        value_path_,
        value_player_,
        value_objective_
    ] = Value->GenResult();

    switch (value_type_)
    {
        case CommandResultType_Value:
            commands.Append("data modify storage {} {}.{} set value {}", Location, object_path_, Key, value_value_);
            break;

        case CommandResultType_Storage:
            commands.Append(
                "data modify storage {} {}.{} set from storage {} {}",
                Location,
                object_path_,
                Key,
                Location,
                value_path_);
            break;

        case CommandResultType_Score:
            commands.Append(
                "execute store result storage {} {}.{} double 1 run scoreboard players get {} {}",
                Location,
                object_path_,
                Key,
                value_player_,
                value_objective_);
            break;
    }
}

mcc::CommandResult mcc::ObjectInstruction::GenResult(const bool stringify) const
{
    throw std::runtime_error("TODO");
}
