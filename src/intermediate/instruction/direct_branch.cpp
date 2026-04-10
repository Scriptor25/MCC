#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::DirectBranchInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const BlockPtr &target)
{
    return Create(where, name, context, parent, target, {}, {});
}

mcc::InstructionPtr mcc::DirectBranchInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const BlockPtr &target,
        const ValuePtr &result,
        const ValuePtr &branch_result)
{
    auto self = std::make_shared<DirectBranchInstruction>(where, name, context, parent, target, result, branch_result);

    self->Self = self;
    self->Target->Use(self);
    if (self->Result)
        self->Result->Use(self);
    if (self->BranchResult)
        self->BranchResult->Use(self);

    return self;
}

mcc::DirectBranchInstruction::DirectBranchInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent,
        BlockPtr target,
        ValuePtr result,
        ValuePtr branch_result)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Parent(std::move(parent)),
      Target(std::move(target)),
      Result(std::move(result)),
      BranchResult(std::move(branch_result))
{
}

mcc::DirectBranchInstruction::~DirectBranchInstruction()
{
    Target->Drop(Self);
    if (Result)
        Result->Drop(Self);
    if (BranchResult)
        BranchResult->Drop(Self);
}

void mcc::DirectBranchInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    if (Result)
    {
        auto branch_result = BranchResult->GenerateResult();
        auto result        = Result->GenerateResult();

        Assert(branch_result.Type == ResultType_::Reference,
               Where,
               "branch result must be {}, but is {}",
               ResultType_::Reference,
               branch_result.Type);

        std::string prefix;
        if (branch_result.WithArgument || result.WithArgument)
            prefix = "$";

        switch (result.Type)
        {
        case ResultType_::Value:
            commands.Append(
                    "{}data modify {} {} {} set value {}",
                    prefix,
                    branch_result.ReferenceType,
                    branch_result.Target,
                    branch_result.Path,
                    result.Value);
            break;

        case ResultType_::Reference:
            commands.Append(
                    "{}data modify {} {} {} set from {} {} {}",
                    prefix,
                    branch_result.ReferenceType,
                    branch_result.Target,
                    branch_result.Path,
                    result.ReferenceType,
                    result.Target,
                    result.Path);
            break;

        case ResultType_::Argument:
            commands.Append(
                    "$data modify {} {} {} set value {}",
                    branch_result.ReferenceType,
                    branch_result.Target,
                    branch_result.Path,
                    result.Name);

        default:
            Error(Where,
                  "result must be {}, {} or {}, but is {}",
                  ResultType_::Value,
                  ResultType_::Reference,
                  ResultType_::Argument,
                  result.Type);
        }
    }

    std::string prefix, arguments;
    Parent->ForwardArguments(prefix, arguments);

    commands.Append("{}return run function {}{}", prefix, Target->GetLocation(), arguments);
}

bool mcc::DirectBranchInstruction::RequireStack() const
{
    return (Result && Result->RequireStack()) || (BranchResult && BranchResult->RequireStack());
}

bool mcc::DirectBranchInstruction::IsTerminator() const
{
    return true;
}
