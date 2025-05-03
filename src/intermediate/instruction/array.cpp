#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(ResourceLocation location, ValuePtr array, ValuePtr value)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Append,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(ResourceLocation location, ValuePtr array, ValuePtr value)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Prepend,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    IndexT index)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Insert,
        std::move(location),
        std::move(array),
        std::move(value),
        index);
}

mcc::ArrayInstruction::ArrayInstruction(
    const ArrayOperationE array_operation,
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const IndexT index)
    : ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index)
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
}

void mcc::ArrayInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::ArrayInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
