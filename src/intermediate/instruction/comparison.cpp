#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ComparisonInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ComparatorE &comparator,
    const ResourceLocation &location,
    const ValuePtr &left,
    const ValuePtr &right)
{
    return std::make_shared<ComparisonInstruction>(where, context, comparator, location, left, right);
}

mcc::ComparisonInstruction::ComparisonInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const ComparatorE comparator,
    const ResourceLocation &location,
    const ValuePtr &left,
    const ValuePtr &right)
    : Instruction(where, context, context.GetBoolean()),
      Comparator(comparator),
      Location(location),
      Left(left),
      Right(right)
{
    Left->Use();
    Right->Use();
}

mcc::ComparisonInstruction::~ComparisonInstruction()
{
    Left->Drop();
    Right->Drop();
}

void mcc::ComparisonInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto left = Left->GenerateResult(false);
    auto right = Right->GenerateResult(false);

    auto require_right = Left != Right && left != right;

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
                Where,
                "left must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                left.Type);
    }

    if (require_right)
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
                    Where,
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
            Error(Where, "undefined comparator {}", Comparator);
    }

    Assert(stack, Where, "comparison instruction requires stack");
    commands.Append(
        "execute store result storage {} {} byte 1 if score %a {} {} {} {}",
        Location,
        GetStackPath(),
        GetTmpName(),
        operator_,
        require_right ? "%b" : "%a",
        GetTmpName());

    commands.Append(RemoveTmpScore());
}

bool mcc::ComparisonInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::ComparisonInstruction::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
