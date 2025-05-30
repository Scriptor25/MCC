#include <utility>
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
    return std::make_shared<BranchInstruction>(where, context, location, condition, then_target, else_target);
}

mcc::BranchInstruction::BranchInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr condition,
    BlockPtr then_target,
    BlockPtr else_target)
    : Instruction(where, context.GetVoid(), false),
      Location(std::move(location)),
      Condition(std::move(condition)),
      ThenTarget(std::move(then_target)),
      ElseTarget(std::move(else_target))
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
    auto temp = GetTemp();

    auto then_target = ThenTarget->GetLocation();
    auto else_target = ElseTarget->GetLocation();

    switch (auto condition = Condition->GenerateResult(); condition.Type)
    {
    case ResultType_Value:
        commands.Append("{}return run function {}{}", prefix, condition.NotNull ? then_target : else_target, arguments);
        break;

    case ResultType_Reference:
        commands.Append(CreateScore());
        commands.Append(
            "{}execute store result score %c {} run data get {} {} {}",
            condition.WithArgument ? "$" : "",
            temp,
            condition.ReferenceType,
            condition.Target,
            condition.Path);
        commands.Append("data remove storage {} {}", Location, stack_path);
        commands.Append(
            "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
            temp,
            Location,
            stack_path);
        commands.Append(RemoveScore());

        commands.Append(
            "{}execute if data storage {} {} run return run function {}{}",
            prefix,
            Location,
            stack_path,
            then_target,
            arguments);
        commands.Append("{}return run function {}{}", prefix, else_target, arguments);
        break;

    case ResultType_Argument:
        commands.Append(CreateScore());
        commands.Append("$scoreboard players set %c {} {}", temp, condition.Name);
        commands.Append("data remove storage {} {}", Location, stack_path);
        commands.Append(
            "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
            temp,
            Location,
            stack_path);
        commands.Append(RemoveScore());

        commands.Append(
            "{}execute if data storage {} {} run return run function {}{}",
            prefix,
            Location,
            stack_path,
            then_target,
            arguments);
        commands.Append("{}return run function {}{}", prefix, else_target, arguments);
        break;

    default:
        Error(
            Where,
            "condition must be {}, {} or {}, but is {}",
            ResultType_Value,
            ResultType_Reference,
            ResultType_Argument,
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
