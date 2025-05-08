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
    Left->Use();
    Right->Use();
}

mcc::OperationInstruction::~OperationInstruction()
{
    Left->Drop();
    Right->Drop();
}

void mcc::OperationInstruction::Generate(CommandVector &commands) const
{
    auto left = Left->GenerateResult(false);
    auto right = Right->GenerateResult(false);

    commands.Append(CreateTmpScore());

    switch (left.Type)
    {
        case ResultType_Value:
            commands.Append("scoreboard players set %a {} {}", GetTmpName(), left.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "execute store result score %a {} run data get storage {} {}",
                GetTmpName(),
                left.Location,
                left.Path);
            break;

        case ResultType_Score:
            commands.Append(
                "scoreboard players operation %a {} = {} {}",
                GetTmpName(),
                left.Player,
                left.Objective);
            break;

        default:
            Error(
                "left must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                left.Type);
    }

    switch (right.Type)
    {
        case ResultType_Value:
            commands.Append("scoreboard players set %b {} {}", GetTmpName(), right.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "execute store result score %b {} run data get storage {} {}",
                GetTmpName(),
                right.Location,
                right.Path);
            break;

        case ResultType_Score:
            commands.Append(
                "scoreboard players operation %b {} = {} {}",
                GetTmpName(),
                right.Player,
                right.Objective);
            break;

        default:
            Error(
                "right must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                right.Type);
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

    commands.Append("scoreboard players operation %a {} {} %b {}", GetTmpName(), operator_, GetTmpName());

    commands.Append(
        "execute store result storage {} {} double 1 run scoreboard players get %a {}",
        Location,
        GetStackPath(),
        GetTmpName());

    commands.Append(RemoveTmpScore());
}

mcc::Result mcc::OperationInstruction::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
