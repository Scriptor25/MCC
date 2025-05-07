#include <mcc/error.hpp>
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
    Left->Use();
    Right->Use();
}

mcc::ComparisonInstruction::~ComparisonInstruction()
{
    Left->Drop();
    Right->Drop();
}

void mcc::ComparisonInstruction::Generate(CommandVector &commands, bool use_stack) const
{
    Assert(use_stack, "comparison instruction requires stack usage");

    auto left = Left->GenerateResult(false, use_stack);
    auto right = Right->GenerateResult(false, use_stack);

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
            Error("undefined comparator {}", Comparator);
    }

    commands.Append(
        "execute store result storage {} {} byte 1 if score %a {} {} %b {}",
        Location,
        GetStackPath(),
        GetTmpName(),
        operator_,
        GetTmpName());

    commands.Append(RemoveTmpScore());
}

mcc::Result mcc::ComparisonInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    Assert(use_stack, "comparison instruction requires stack usage");

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}

bool mcc::ComparisonInstruction::RequireStack() const
{
    return true;
}
