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
}

void mcc::ObjectInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::ObjectInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
