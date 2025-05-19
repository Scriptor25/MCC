#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::DirectInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const BlockPtr &target)
{
    return std::make_shared<DirectInstruction>(
        where,
        location,
        target,
        nullptr,
        nullptr);
}

mcc::InstructionPtr mcc::DirectInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const BlockPtr &target,
    const ValuePtr &result,
    const ValuePtr &branch_result)
{
    return std::make_shared<DirectInstruction>(
        where,
        location,
        target,
        result,
        branch_result);
}

mcc::DirectInstruction::DirectInstruction(
    const SourceLocation &where,
    const ResourceLocation &location,
    const BlockPtr &target,
    const ValuePtr &result,
    const ValuePtr &branch_result)
    : Instruction(where, TypeContext::GetVoid()),
      Location(location),
      Target(target),
      Result(result),
      BranchResult(branch_result)
{
    Target->Use();
    if (Result)
        Result->Use();
    if (BranchResult)
        BranchResult->Use();
}

mcc::DirectInstruction::~DirectInstruction()
{
    Target->Drop();
    if (Result)
        Result->Drop();
    if (BranchResult)
        BranchResult->Drop();
}

void mcc::DirectInstruction::Generate(CommandVector &commands, bool stack) const
{
    if (Result)
    {
        auto branch_result = BranchResult->GenerateResult(false);

        Assert(
            branch_result.Type == ResultType_Storage,
            Where,
            "branch result must be {}, but is {}",
            ResultType_Storage,
            branch_result.Type);

        switch (auto result = Result->GenerateResult(false); result.Type)
        {
            case ResultType_Value:
                commands.Append(
                    "data modify storage {} {} set value {}",
                    branch_result.Location,
                    branch_result.Path,
                    result.Value);
                break;

            case ResultType_Storage:
                commands.Append(
                    "data modify storage {} {} set from storage {} {}",
                    branch_result.Location,
                    branch_result.Path,
                    result.Location,
                    result.Path);
                break;

            case ResultType_Score:
                commands.Append(
                    "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                    branch_result.Location,
                    branch_result.Path,
                    result.Player,
                    result.Objective);
                break;

            default:
                Error(
                    Where,
                    "result must be {}, {} or {}, but is {}",
                    ResultType_Value,
                    ResultType_Storage,
                    ResultType_Score,
                    result.Type);
        }
    }

    std::string prefix, arguments;
    Target->Parent->ForwardArguments(prefix, arguments);

    commands.Append("{}return run function {}{}", prefix, Target->Parent->GetLocation(Target), arguments);
}

bool mcc::DirectInstruction::RequireStack() const
{
    return Result ? Result->RequireStack() || BranchResult->RequireStack() : false;
}

bool mcc::DirectInstruction::IsTerminator() const
{
    return true;
}
