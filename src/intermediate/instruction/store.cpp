#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(ValuePtr dst, ValuePtr src)
{
    return std::make_shared<StoreInstruction>(std::move(dst), std::move(src));
}

mcc::StoreInstruction::StoreInstruction(ValuePtr dst, ValuePtr src)
    : Dst(std::move(dst)),
      Src(std::move(src))
{
    Dst->Use();
    Src->Use();
}

mcc::StoreInstruction::~StoreInstruction()
{
    Dst->Drop();
    Src->Drop();
}

void mcc::StoreInstruction::Generate(CommandVector &commands) const
{
    if (Dst == Src)
        return;

    auto dst = Dst->GenerateResult(false);
    auto src = Src->GenerateResult(false);

    switch (dst.Type)
    {
        case ResultType_Storage:
            switch (src.Type)
            {
                case ResultType_Value:
                    commands.Append("data modify storage {} {} set value {}", dst.Location, dst.Path, src.Value);
                    break;

                case ResultType_Storage:
                    if (dst.Location == src.Location && dst.Path == src.Path)
                        break;
                    commands.Append(
                        "data modify storage {} {} set from storage {} {}",
                        dst.Location,
                        dst.Path,
                        src.Location,
                        src.Path);
                    break;

                case ResultType_Score:
                    commands.Append(
                        "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                        dst.Location,
                        dst.Path,
                        src.Player,
                        src.Objective);
                    break;

                default:
                    Error(
                        "src must be {}, {} or {}, but is {}",
                        ResultType_Value,
                        ResultType_Storage,
                        ResultType_Score,
                        src.Type);
            }
            break;

        case ResultType_Score:
            switch (src.Type)
            {
                case ResultType_Value:
                    commands.Append("scoreboard players set {} {} {}", dst.Player, dst.Objective, src.Value);
                    break;

                case ResultType_Storage:
                    commands.Append(
                        "execute store result score {} {} run data get storage {} {}",
                        dst.Player,
                        dst.Objective,
                        src.Location,
                        src.Path);
                    break;

                case ResultType_Score:
                    if (dst.Player == src.Player && dst.Objective == src.Objective)
                        break;
                    commands.Append(
                        "scoreboard players operation {} {} = {} {}",
                        dst.Player,
                        dst.Objective,
                        src.Player,
                        src.Objective);
                    break;

                default:
                    Error(
                        "src must be {}, {} or {}, but is {}",
                        ResultType_Value,
                        ResultType_Storage,
                        ResultType_Score,
                        src.Type);
            }
            break;

        default:
            Error(
                "dst must be {} or {}, but is {}",
                ResultType_Storage,
                ResultType_Score,
                dst.Type);
    }
}

mcc::Result mcc::StoreInstruction::GenerateResult(const bool stringify) const
{
    return Dst->GenerateResult(stringify);
}
