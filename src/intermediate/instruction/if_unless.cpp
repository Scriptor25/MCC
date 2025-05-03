#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::IfUnlessInstruction::Create(
    ResourceLocation location,
    bool unless,
    ValuePtr condition,
    ValuePtr then,
    ValuePtr else_)
{
    return std::make_shared<IfUnlessInstruction>(
        std::move(location),
        unless,
        std::move(condition),
        std::move(then),
        std::move(else_));
}

mcc::IfUnlessInstruction::IfUnlessInstruction(
    ResourceLocation location,
    const bool unless,
    ValuePtr condition,
    ValuePtr then,
    ValuePtr else_)
    : Location(std::move(location)),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

void mcc::IfUnlessInstruction::Gen(CommandVector &commands) const
{
    auto [
        condition_type_,
        condition_value_,
        condition_path_,
        condition_player_,
        condition_objective_
    ] = Condition->GenResult();

    auto then = Then->GenInline();
    auto else_ = Else->GenInline();

    switch (condition_type_)
    {
        case CommandResultType_Value:
            if (Unless == (condition_value_ == "false" || condition_value_ == "0"))
                Then->Gen(commands);
            else
                Else->Gen(commands);
            break;

        case CommandResultType_Storage:
            commands.Append("scoreboard objectives add tmp dummy");
            commands.Append(
                "execute store result score %c tmp run data get storage {} {}",
                Location,
                condition_path_);
            commands.Append(
                "execute {} score %c tmp matches 0 run execute store result storage {} stack[0].result.{} double 1 run {}",
                Unless ? "if" : "unless",
                Location,
                GetResultID(),
                then);
            commands.Append(
                "execute {} score %c tmp matches 0 run execute store result storage {} stack[0].result.{} double 1 run {}",
                Unless ? "unless" : "if",
                Location,
                GetResultID(),
                else_);
            commands.Append("scoreboard objectives remove tmp");
            break;

        case CommandResultType_Score:
            commands.Append(
                "execute {} score {} {} matches 0 run execute store result storage {} stack[0].result.{} double 1 run {}",
                Unless ? "if" : "unless",
                condition_player_,
                condition_objective_,
                Location,
                GetResultID(),
                then);
            commands.Append(
                "execute {} score {} {} matches 0 run execute store result storage {} stack[0].result.{} double 1 run {}",
                Unless ? "unless" : "if",
                condition_player_,
                condition_objective_,
                Location,
                GetResultID(),
                else_);
            break;
    }
}

mcc::CommandResult mcc::IfUnlessInstruction::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].result." + GetResultID(),
    };
}
