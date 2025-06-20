#include <utility>
#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::DirectBranchInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const BlockPtr &target)
{
    return std::make_shared<DirectBranchInstruction>(where, context, location, target, nullptr, nullptr);
}

mcc::InstructionPtr mcc::DirectBranchInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const BlockPtr &target,
    const ValuePtr &result,
    const ValuePtr &branch_result)
{
    return std::make_shared<DirectBranchInstruction>(where, context, location, target, result, branch_result);
}

mcc::DirectBranchInstruction::DirectBranchInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    BlockPtr target,
    ValuePtr result,
    ValuePtr branch_result)
    : Instruction(where, context.GetVoid(), FieldType_Value),
      Location(std::move(location)),
      Target(std::move(target)),
      Result(std::move(result)),
      BranchResult(std::move(branch_result))
{
    Target->Use();
    if (Result)
        Result->Use();
    if (BranchResult)
        BranchResult->Use();
}

mcc::DirectBranchInstruction::~DirectBranchInstruction()
{
    Target->Drop();
    if (Result)
        Result->Drop();
    if (BranchResult)
        BranchResult->Drop();
}

void mcc::DirectBranchInstruction::Generate(CommandVector &commands, bool stack) const
{
    if (Result)
    {
        auto branch_result = BranchResult->GenerateResult();
        auto result = Result->GenerateResult();

        Assert(
            branch_result.Type == ResultType_Reference,
            Where,
            "branch result must be {}, but is {}",
            ResultType_Reference,
            branch_result.Type);

        std::string prefix;
        if (branch_result.WithArgument || result.WithArgument)
            prefix = "$";

        switch (result.Type)
        {
        case ResultType_Value:
            commands.Append(
                "{}data modify {} {} {} set value {}",
                prefix,
                branch_result.ReferenceType,
                branch_result.Target,
                branch_result.Path,
                result.Value);
            break;

        case ResultType_Reference:
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

        case ResultType_Argument:
            commands.Append(
                "$data modify {} {} {} set value {}",
                branch_result.ReferenceType,
                branch_result.Target,
                branch_result.Path,
                result.Name);

        default:
            Error(
                Where,
                "result must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Reference,
                ResultType_Argument,
                result.Type);
        }
    }

    std::string prefix, arguments;
    Target->Parent->ForwardArguments(prefix, arguments);

    commands.Append("{}return run function {}{}", prefix, Target->GetLocation(), arguments);
}

bool mcc::DirectBranchInstruction::RequireStack() const
{
    return Result ? Result->RequireStack() : (BranchResult ? BranchResult->RequireStack() : false);
}

bool mcc::DirectBranchInstruction::IsTerminator() const
{
    return true;
}
