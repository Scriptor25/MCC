#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(ResourceLocation location, ValuePtr value)
{
    return std::make_shared<ReturnInstruction>(std::move(location), std::move(value));
}

mcc::ReturnInstruction::ReturnInstruction(ResourceLocation location, ValuePtr value)
    : Location(std::move(location)),
      Value(std::move(value))
{
}

void mcc::ReturnInstruction::Gen(CommandVector &commands) const
{
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
            commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return {}", value_value_);
            break;

        case CommandResultType_Storage:
            commands.Append("data modify storage {0} result set from storage {0} {1}", Location, value_path_);
            commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return run data get storage {} result", Location);
            break;

        case CommandResultType_Score:
            commands.Append("data remove storage {} stack[0]", Location);
            commands.Append("return run scoreboard players get {} {}", value_player_, value_objective_);
            break;
    }
}

mcc::CommandT mcc::ReturnInstruction::GenInline() const
{
    return "return run " + Value->GenInline();
}
