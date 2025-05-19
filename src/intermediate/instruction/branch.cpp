#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::BranchInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &condition,
    const BlockPtr &then_target,
    const BlockPtr &else_target)
{
    return std::make_shared<BranchInstruction>(
        where,
        context,
        location,
        condition,
        then_target,
        else_target);
}

mcc::BranchInstruction::BranchInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &condition,
    const BlockPtr &then_target,
    const BlockPtr &else_target)
    : Instruction(where, context, context.GetVoid()),
      Location(location),
      Condition(condition),
      ThenTarget(then_target),
      ElseTarget(else_target)
{
    Condition->Use();
    ThenTarget->Use();
    ElseTarget->Use();
}

mcc::BranchInstruction::~BranchInstruction()
{
    Condition->Drop();
    ThenTarget->Drop();
    ElseTarget->Drop();
}

void mcc::BranchInstruction::Generate(CommandVector &commands, bool stack) const
{
    std::string prefix, arguments;
    ThenTarget->Parent->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto tmp_name = GetTmpName();

    auto then = ThenTarget->Parent->GetLocation(ThenTarget);
    auto else_ = ElseTarget->Parent->GetLocation(ElseTarget);

    switch (auto condition = Condition->GenerateResult(false); condition.Type)
    {
        case ResultType_Value:
            commands.Append(
                "{}return run function {}{}",
                prefix,
                condition.Value == "false" || condition.Value == "0"
                    ? else_
                    : then,
                arguments);
            break;

        case ResultType_Storage:
            commands.Append(CreateTmpScore());
            commands.Append(
                "execute store result score %c {} run data get storage {} {}",
                tmp_name,
                condition.Location,
                condition.Path);
            commands.Append(
                "data remove storage {} {}",
                Location,
                stack_path);
            commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
                tmp_name,
                Location,
                stack_path);
            commands.Append(RemoveTmpScore());

            commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                Location,
                stack_path,
                then,
                arguments);
            commands.Append(
                "{}return run function {}{}",
                prefix,
                else_,
                arguments);
            break;

        case ResultType_Score:
            commands.Append(
                "execute unless score {} {} matches 0 run return run function {}{}",
                condition.Player,
                condition.Objective,
                then,
                arguments);
            commands.Append(
                "{}return run function {}{}",
                prefix,
                else_,
                arguments);
            break;

        default:
            Error(
                Where,
                "condition must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                condition.Type);
    }
}

bool mcc::BranchInstruction::RequireStack() const
{
    return true;
}

bool mcc::BranchInstruction::IsTerminator() const
{
    return true;
}
