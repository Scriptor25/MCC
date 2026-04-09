#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ComparisonInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const Comparator_ &comparator,
        const ResourceLocation &location,
        const ValuePtr &left,
        const ValuePtr &right)
{
    auto self = std::make_shared<ComparisonInstruction>(where, name, context, comparator, location, left, right);

    self->Self = self;
    self->Left->Use(self);
    self->Right->Use(self);

    return self;
}

mcc::ComparisonInstruction::ComparisonInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const Comparator_ comparator,
        ResourceLocation location,
        ValuePtr left,
        ValuePtr right)
    : Instruction(
              where,
              name,
              context.GetNumber(),
              FieldType_::Value),
      Comparator(comparator),
      Location(std::move(location)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

mcc::ComparisonInstruction::~ComparisonInstruction()
{
    Left->Drop(Self);
    Right->Drop(Self);
}

void mcc::ComparisonInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto left  = Left->GenerateResult();
    auto right = Right->GenerateResult();

    const auto require_right = Left != Right && left != right;

    auto objective = GetTemp();

    commands.Append(CreateScore());

    std::string prefix;
    if (left.WithArgument)
        prefix = "$";

    switch (left.Type)
    {
    case ResultType_::Value:
        commands.Append("{}scoreboard players set %a {} {}", prefix, objective, left.Value);
        break;

    case ResultType_::Reference:
        commands.Append(
                "{}execute store result score %a {} run data get {} {} {}",
                prefix,
                objective,
                left.ReferenceType,
                left.Target,
                left.Path);
        break;

    case ResultType_::Argument:
        commands.Append("$scoreboard players set %a {} {}", objective, left.Name);
        break;

    default:
        Error(Where,
              "left must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
              left.Type);
    }

    if (require_right)
    {
        prefix = "";
        if (right.WithArgument)
            prefix = "$";

        switch (right.Type)
        {
        case ResultType_::Value:
            commands.Append("{}scoreboard players set %b {} {}", prefix, objective, right.Value);
            break;

        case ResultType_::Reference:
            commands.Append(
                    "{}execute store result score %b {} run data get {} {} {}",
                    prefix,
                    objective,
                    right.ReferenceType,
                    right.Target,
                    right.Path);
            break;

        case ResultType_::Argument:
            commands.Append("$scoreboard players set %b {} {}", objective, right.Name);
            break;

        default:
            Error(Where,
                  "right must be {}, {} or {}, but is {}",
                  ResultType_::Value,
                  ResultType_::Reference,
                  ResultType_::Argument,
                  right.Type);
        }
    }

    std::string operator_;
    switch (Comparator)
    {
    case Comparator_::LT:
        operator_ = "<";
        break;

    case Comparator_::GT:
        operator_ = ">";
        break;

    case Comparator_::LE:
        operator_ = "<=";
        break;

    case Comparator_::GE:
        operator_ = ">=";
        break;

    case Comparator_::EQ:
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

    commands.Append(RemoveScore());
}

bool mcc::ComparisonInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::ComparisonInstruction::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Location.String(),
        .Path          = GetStackPath(),
    };
}
