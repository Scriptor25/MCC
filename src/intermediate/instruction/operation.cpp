#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::OperationInstruction::Create(
    const OperatorE operator_,
    ResourceLocation location,
    ValuePtr left,
    ValuePtr right)
{
    return std::make_shared<OperationInstruction>(operator_, std::move(location), std::move(left), std::move(right));
}

mcc::OperationInstruction::OperationInstruction(
    const OperatorE operator_,
    ResourceLocation location,
    ValuePtr left,
    ValuePtr right)
    : Operator(operator_),
      Location(std::move(location)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

void mcc::OperationInstruction::Gen(CommandVector &commands) const
{
    auto [
        left_type_,
        left_value_,
        left_path_,
        left_player_,
        left_objective_
    ] = Left->GenResult();
    auto [
        right_type_,
        right_value_,
        right_path_,
        right_player_,
        right_objective_
    ] = Right->GenResult();

    commands.Append("scoreboard objectives add tmp dummy");

    switch (left_type_)
    {
        case CommandResultType_Value:
            commands.Append("scoreboard players set %a tmp {}", left_value_);
            break;

        case CommandResultType_Storage:
            commands.Append(
                "execute store result score %a tmp run data get storage {} {}",
                Location,
                left_path_);
            break;

        case CommandResultType_Score:
            commands.Append(
                "scoreboard players operation %a tmp = {} {}",
                left_player_,
                left_objective_);
            break;
    }
    switch (right_type_)
    {
        case CommandResultType_Value:
            commands.Append("scoreboard players set %b tmp {}", right_value_);
            break;

        case CommandResultType_Storage:
            commands.Append(
                "execute store result score %b tmp run data get storage {} {}",
                Location,
                right_path_);
            break;

        case CommandResultType_Score:
            commands.Append(
                "scoreboard players operation %b tmp = {} {}",
                right_player_,
                right_objective_);
            break;
    }

    std::string operator_;
    switch (Operator)
    {
        case Operator_Add:
            operator_ = "+=";
            break;

        case Operator_Sub:
            operator_ = "-=";
            break;

        case Operator_Mul:
            operator_ = "*=";
            break;

        case Operator_Div:
            operator_ = "/=";
            break;

        case Operator_Rem:
            operator_ = "%=";
            break;

        default:
            Error("undefined operator {}", Operator);
    }

    commands.Append("scoreboard players operation %a tmp {} %b tmp", operator_);
    commands.Append(
        "execute store result storage {} stack[0].result.{} double 1 run scoreboard players get %a tmp",
        Location,
        GetResultID());
    commands.Append("scoreboard objectives remove tmp");
}

mcc::CommandResult mcc::OperationInstruction::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].result." + GetResultID(),
    };
}
