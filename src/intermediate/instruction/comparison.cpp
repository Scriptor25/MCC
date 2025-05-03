#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ComparisonInstruction::Create(
    ComparatorE comparator,
    ResourceLocation location,
    ValuePtr left,
    ValuePtr right)
{
    return std::make_shared<ComparisonInstruction>(comparator, std::move(location), std::move(left), std::move(right));
}

mcc::ComparisonInstruction::ComparisonInstruction(
    const ComparatorE comparator,
    ResourceLocation location,
    ValuePtr left,
    ValuePtr right)
    : Comparator(comparator),
      Location(std::move(location)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

void mcc::ComparisonInstruction::Gen(CommandVector &commands) const
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
    switch (Comparator)
    {
        case Comparator_LT:
            operator_ = "<";
            break;

        case Comparator_GT:
            operator_ = ">";
            break;

        case Comparator_LE:
            operator_ = "<=";
            break;

        case Comparator_GE:
            operator_ = ">=";
            break;

        case Comparator_EQ:
            operator_ = "=";
            break;

        default:
            throw std::runtime_error("TODO");
    }

    commands.Append(
        "execute store result storage {} stack[0].result.{} byte 1 if score %a tmp {} %b tmp",
        Location,
        GetResultID(),
        operator_);
    commands.Append("scoreboard objectives remove tmp");
}

mcc::CommandResult mcc::ComparisonInstruction::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].result." + GetResultID(),
    };
}
