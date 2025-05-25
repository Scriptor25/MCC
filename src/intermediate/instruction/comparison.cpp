#include <utility>
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
    ResourceLocation location,
    ValuePtr left,
    ValuePtr right)
    : Instruction(where, context.GetBoolean(), false),
      Comparator(comparator),
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

void mcc::ComparisonInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto left = Left->GenerateResult(false);
    auto right = Right->GenerateResult(false);

    const auto require_right = Left != Right && left != right;

    auto objective = GetTmpName();

    commands.Append(CreateTmpScore());

    switch (left.Type)
    {
        case ResultType_Value:
            commands.Append("scoreboard players set %a {} {}", objective, left.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "execute store result score %a {} run data get storage {} {}",
                objective,
                left.Location,
                left.Path);
            break;

        case ResultType_Argument:
            commands.Append("$scoreboard players set %a {} {}", objective, left.Name);
            break;

        default:
            Error(
                Where,
                "left must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Argument,
                left.Type);
    }

    if (require_right)
    {
        switch (right.Type)
        {
            case ResultType_Value:
                commands.Append("scoreboard players set %b {} {}", objective, right.Value);
                break;

            case ResultType_Storage:
                commands.Append(
                    "execute store result score %b {} run data get storage {} {}",
                    objective,
                    right.Location,
                    right.Path);
                break;

            case ResultType_Argument:
                commands.Append("$scoreboard players set %b {} {}", objective, right.Name);
                break;

            default:
                Error(
                    Where,
                    "right must be {}, {} or {}, but is {}",
                    ResultType_Value,
                    ResultType_Storage,
                    ResultType_Argument,
                    right.Type);
        }
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
        objective,
        operator_,
        require_right ? "%b" : "%a",
        objective);

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
