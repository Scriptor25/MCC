#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::OperationInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const Operator_ operator_,
        const ResourceLocation &location,
        const std::vector<ValuePtr> &operands)
{
    auto self = std::make_shared<OperationInstruction>(where, name, context, operator_, location, operands);

    self->Self = self;
    for (const auto &operand : self->Operands)
        operand->Use(self);

    return self;
}

mcc::OperationInstruction::OperationInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const Operator_ operator_,
        ResourceLocation location,
        const std::vector<ValuePtr> &operands)
    : Instruction(
              where,
              name,
              context.GetNumber(),
              FieldType_::Value),
      Operator(operator_),
      Location(std::move(location)),
      Operands(operands)
{
}

mcc::OperationInstruction::~OperationInstruction()
{
    for (const auto &operand : Operands)
        operand->Drop(Self);
}

void mcc::OperationInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    auto objective = GetTemp();

    commands.Append(CreateScore());

    std::string operator_;
    switch (Operator)
    {
    case Operator_::Add:
        operator_ = "+=";
        break;

    case Operator_::Sub:
        operator_ = "-=";
        break;

    case Operator_::Mul:
        operator_ = "*=";
        break;

    case Operator_::Div:
        operator_ = "/=";
        break;

    case Operator_::Rem:
        operator_ = "%=";
        break;

    default:
        Error(Where, "undefined operator {}", Operator);
    }

    ValuePtr pre_operand_value;
    Result pre_operand;

    for (unsigned i = 0; i < Operands.size(); ++i)
    {
        auto player = i == 0 ? "%a" : "%b";

        auto operand_value = Operands[i];
        auto operand       = operand_value->GenerateResult();

        auto require_operand = operand_value != pre_operand_value && operand != pre_operand;

        pre_operand_value = operand_value;
        pre_operand       = operand;

        if (require_operand)
        {
            std::string prefix;
            if (operand.WithArgument)
                prefix = "$";

            switch (operand.Type)
            {
            case ResultType_::Value:
                commands.Append("{}scoreboard players set {} {} {}", prefix, player, objective, operand.Value);
                break;

            case ResultType_::Reference:
                commands.Append(
                        "{}execute store result score {} {} run data get {} {} {}",
                        prefix,
                        player,
                        objective,
                        operand.ReferenceType,
                        operand.Target,
                        operand.Path);
                break;

            case ResultType_::Argument:
                commands.Append("$scoreboard players set {} {} {}", player, objective, operand.Name);
                break;

            default:
                Error(Where,
                      "operand must be {}, {} or {}, but is {}",
                      ResultType_::Value,
                      ResultType_::Reference,
                      ResultType_::Argument,
                      operand.Type);
            }
        }

        if (i)
        {
            commands.Append(
                    "scoreboard players operation %a {0} {1} {2} {0}",
                    objective,
                    operator_,
                    require_operand ? "%b"
                    : i > 1         ? "%b"
                                    : "%a");
        }
    }

    Assert(stack, Where, "operation instruction requires stack");
    commands.Append(
            "execute store result storage {} {} long 1 run scoreboard players get %a {}",
            Location,
            GetStackPath(),
            objective);

    commands.Append(RemoveScore());
}

bool mcc::OperationInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::OperationInstruction::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Location.String(),
        .Path          = GetStackPath(),
    };
}
