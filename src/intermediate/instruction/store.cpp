#include <mcc/intermediate.hpp>
#include <mcc/lex.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(ResourceLocation location, ValuePtr dst, ValuePtr src)
{
    return std::make_shared<StoreInstruction>(std::move(location), std::move(dst), std::move(src));
}

mcc::StoreInstruction::StoreInstruction(ResourceLocation location, ValuePtr dst, ValuePtr src)
    : Location(std::move(location)),
      Dst(std::move(dst)),
      Src(std::move(src))
{
}

void mcc::StoreInstruction::Gen(CommandVector &commands) const
{
    auto [
        dst_type_,
        dst_value_,
        dst_path_,
        dst_player_,
        dst_objective_
    ] = Dst->GenResult();
    auto [
        src_type_,
        src_value_,
        src_path_,
        src_player_,
        src_objective_
    ] = Src->GenResult();

    switch (dst_type_)
    {
        case CommandResultType_Storage:
            switch (src_type_)
            {
                case CommandResultType_Value:
                    commands.Append("data modify storage {} {} set value {}", Location, dst_path_, src_value_);
                    break;

                case CommandResultType_Storage:
                    commands.Append(
                        "data modify storage {0} {1} set from storage {0} {2}",
                        Location,
                        dst_path_,
                        src_path_);
                    break;

                case CommandResultType_Score:
                    commands.Append(
                        "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                        Location,
                        dst_path_,
                        src_player_,
                        src_objective_);
                    break;
            }
            break;

        case CommandResultType_Score:
            switch (src_type_)
            {
                case CommandResultType_Value:
                    commands.Append("scoreboard players set {} {} {}", dst_player_, dst_objective_, src_value_);
                    break;

                case CommandResultType_Storage:
                    commands.Append(
                        "execute store result score {} {} run data get storage {} {}",
                        dst_player_,
                        dst_objective_,
                        Location,
                        src_path_);
                    break;

                case CommandResultType_Score:
                    commands.Append(
                        "scoreboard players operation {} {} = {} {}",
                        dst_player_,
                        dst_objective_,
                        src_player_,
                        src_objective_);
                    break;
            }
            break;

        default:
            throw std::runtime_error("TODO");
    }
}

mcc::CommandResult mcc::StoreInstruction::GenResult(const bool stringify) const
{
    return Dst->GenResult();
}
