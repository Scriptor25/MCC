#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::BranchInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const ValuePtr &condition,
        const BlockPtr &then_target,
        const BlockPtr &else_target)
{
    auto self = std::make_shared<BranchInstruction>(where, name, context, parent, condition, then_target, else_target);

    self->Self = self;
    self->Condition->Use(self);
    self->ThenTarget->Use(self);
    self->ElseTarget->Use(self);

    return self;
}

mcc::BranchInstruction::BranchInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent,
        ValuePtr condition,
        BlockPtr then_target,
        BlockPtr else_target)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Parent(std::move(parent)),
      Condition(std::move(condition)),
      ThenTarget(std::move(then_target)),
      ElseTarget(std::move(else_target))
{
}

mcc::BranchInstruction::~BranchInstruction()
{
    Condition->Drop(Self);
    ThenTarget->Drop(Self);
    ElseTarget->Drop(Self);
}

void mcc::BranchInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto location = Parent->Mangle();

    std::string prefix, arguments;
    Parent->ForwardArguments(prefix, arguments);

    auto stack_path = GetStackPath();
    auto temp       = GetTemp();

    auto then_target = ThenTarget->GetLocation();
    auto else_target = ElseTarget->GetLocation();

    switch (auto condition = Condition->GenerateResult(); condition.Type)
    {
    case ResultType_::Value:
        commands.Append("{}return run function {}{}", prefix, condition.NotNull ? then_target : else_target, arguments);
        break;

    case ResultType_::Reference:
        commands.Append(CreateScore());
        commands.Append(
                "{}execute store result score %c {} run data get {} {} {}",
                condition.WithArgument ? "$" : "",
                temp,
                condition.ReferenceType,
                condition.Target,
                condition.Path);
        commands.Append("data remove storage {} {}", location, stack_path);
        commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
                temp,
                location,
                stack_path);
        commands.Append(RemoveScore());

        commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                location,
                stack_path,
                then_target,
                arguments);
        commands.Append("{}return run function {}{}", prefix, else_target, arguments);
        break;

    case ResultType_::Argument:
        commands.Append(CreateScore());
        commands.Append("$scoreboard players set %c {} {}", temp, condition.Name);
        commands.Append("data remove storage {} {}", location, stack_path);
        commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
                temp,
                location,
                stack_path);
        commands.Append(RemoveScore());

        commands.Append(
                "{}execute if data storage {} {} run return run function {}{}",
                prefix,
                location,
                stack_path,
                then_target,
                arguments);
        commands.Append("{}return run function {}{}", prefix, else_target, arguments);
        break;

    default:
        Error(Where,
              "condition must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
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
