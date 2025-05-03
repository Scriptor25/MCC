#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Append,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Prepend,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    IndexT index,
    bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Insert,
        std::move(location),
        std::move(array),
        std::move(value),
        index,
        stringify);
}

mcc::ArrayInstruction::ArrayInstruction(
    const ArrayOperationE array_operation,
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const IndexT index,
    const bool stringify)
    : ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index),
      Stringify(stringify)
{
}

void mcc::ArrayInstruction::Gen(CommandVector &commands) const
{
    //
    // array: stack-allocated
    //
    //      data modify storage <location> stack[0].values[<array.index>] (append|prepend|insert <index>) ...
    //
    // value -> constant:           ... value <value>
    //       -> stack-allocated:    ... from storage <location> stack[0].values[<value.index>]
    //
    // value -> register:
    //
    //      data modify storage <location> stack[0].values[<array.index>] (append|prepend|insert <index>) value 0
    //      execute store result storage <location> stack[0].values[<array.index>][(<back>|0|<index>)] double 1 run scoreboard players get <value.player> <value.objective>
    //

    auto [
        array_type_,
        array_value_,
        array_path_,
        array_player_,
        array_objective_
    ] = Array->GenResult();
    auto [
        value_type_,
        value_value_,
        value_path_,
        value_player_,
        value_objective_
    ] = Value->GenResult(Stringify);

    std::string operation;
    switch (ArrayOperation)
    {
        case ArrayOperation_Append:
            operation = "append";
            break;

        case ArrayOperation_Prepend:
            operation = "prepend";
            break;

        case ArrayOperation_Insert:
            operation = "insert " + std::to_string(Index);
            break;
    }

    switch (value_type_)
    {
        case CommandResultType_Value:
            commands.Append("data modify storage {} {} {} value {}", Location, array_path_, operation, value_value_);
            break;

        case CommandResultType_Storage:
            commands.Append(
                "data modify storage {0} {1} {2} {3} storage {0} {4}",
                Location,
                array_path_,
                operation,
                Stringify ? "string" : "from",
                value_path_);
            break;

        case CommandResultType_Score:
            commands.Append(
                "execute store result storage {} tmp double 1 run scoreboard players get {} {}",
                Location,
                value_player_,
                value_objective_);
            commands.Append(
                "data modify storage {0} {1} {2} {3} storage {0} tmp",
                Location,
                array_path_,
                operation,
                Stringify ? "string" : "from");
            break;
    }
}

mcc::CommandResult mcc::ArrayInstruction::GenResult(const bool stringify) const
{
    throw std::runtime_error("TODO");
}
