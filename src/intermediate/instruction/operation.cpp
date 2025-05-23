#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::OperationInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const OperatorE operator_,
    const ResourceLocation &location,
    const std::vector<ValuePtr> &operands)
{
    return std::make_shared<OperationInstruction>(where, context, operator_, location, operands);
}

mcc::OperationInstruction::OperationInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const OperatorE operator_,
    const ResourceLocation &location,
    const std::vector<ValuePtr> &operands)
    : Instruction(where, context, context.GetNumber()),
      Operator(operator_),
      Location(location),
      Operands(operands)
{
    for (const auto &operand: Operands)
        operand->Use();
}

mcc::OperationInstruction::~OperationInstruction()
{
    for (const auto &operand: Operands)
        operand->Drop();
}

void mcc::OperationInstruction::Generate(CommandVector &commands, const bool stack) const
{
    auto objective = GetTmpName();

    commands.Append(CreateTmpScore());

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
            Error(Where, "undefined operator {}", Operator);
    }

    ValuePtr pre_operand_value;
    Result pre_operand;

    for (unsigned i = 0; i < Operands.size(); ++i)
    {
        auto player = i == 0 ? "%a" : "%b";

        auto operand_value = Operands[i];
        auto operand = operand_value->GenerateResult(false);

        auto require_operand = operand_value != pre_operand_value && operand != pre_operand;

        pre_operand_value = operand_value;
        pre_operand = operand;

        if (require_operand)
        {
            switch (operand.Type)
            {
                case ResultType_Value:
                    commands.Append(
                        "scoreboard players set {} {} {}",
                        player,
                        objective,
                        operand.Value);
                    break;

                case ResultType_Storage:
                    commands.Append(
                        "execute store result score {} {} run data get storage {} {}",
                        player,
                        objective,
                        operand.Location,
                        operand.Path);
                    break;

                default:
                    Error(
                        Where,
                        "operand must be {} or {}, but is {}",
                        ResultType_Value,
                        ResultType_Storage,
                        operand.Type);
            }
        }

        if (i)
        {
            commands.Append(
                "scoreboard players operation %a {0} {1} {2} {0}",
                objective,
                operator_,
                require_operand ? "%b" : i > 1 ? "%b" : "%a");
        }
    }

    Assert(stack, Where, "operation instruction requires stack");
    commands.Append(
        "execute store result storage {} {} double 1 run scoreboard players get %a {}",
        Location,
        GetStackPath(),
        objective);

    commands.Append(RemoveTmpScore());
}

bool mcc::OperationInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::OperationInstruction::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
