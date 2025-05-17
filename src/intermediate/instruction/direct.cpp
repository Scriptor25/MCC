#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::DirectInstruction::Create(ResourceLocation location, BlockPtr target)
{
    return std::make_shared<DirectInstruction>(std::move(location), std::move(target), nullptr, nullptr);
}

mcc::InstructionPtr mcc::DirectInstruction::Create(
    ResourceLocation location,
    BlockPtr target,
    ValuePtr result,
    ValuePtr branch_result)
{
    return std::make_shared<DirectInstruction>(
        std::move(location),
        std::move(target),
        std::move(result),
        std::move(branch_result));
}

mcc::DirectInstruction::DirectInstruction(
    ResourceLocation location,
    BlockPtr target,
    ValuePtr result,
    ValuePtr branch_result)
    : Location(std::move(location)),
      Target(std::move(target)),
      Result(std::move(result)),
      BranchResult(std::move(branch_result))
{
    Target->Use();
    if (Result)
        Result->Use();
}

mcc::DirectInstruction::~DirectInstruction()
{
    Target->Drop();
    if (Result)
        Result->Drop();
}

void mcc::DirectInstruction::Generate(CommandVector &commands, bool stack) const
{
    if (Result)
    {
        auto result = Result->GenerateResult(false);
        auto branch_result = BranchResult->GenerateResult(false);

        Assert(
            branch_result.Type == ResultType_Storage,
            "branch result must be {}, but is {}",
            ResultType_Storage,
            branch_result.Type);

        switch (result.Type)
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
                    "result must be {}, {} or {}, but is {}",
                    ResultType_Value,
                    ResultType_Storage,
                    ResultType_Score,
                    result.Type);
        }
    }

    std::string arguments;
    std::string prefix;

    if (auto &parameters = Target->Parent->Parameters; !parameters.empty())
    {
        prefix = "$";
        arguments += " {";
        for (unsigned i = 0; i < parameters.size(); ++i)
        {
            if (i)
                arguments += ',';
            arguments += std::format("{0}:$({0})", parameters[i]);
        }
        arguments += '}';
    }

    auto target = Target->Location;
    commands.Append("{}return run function {}{}", prefix, target, arguments);
}

bool mcc::DirectInstruction::RequireStack() const
{
    return Result ? Result->RequireStack() || BranchResult->RequireStack() : false;
}

bool mcc::DirectInstruction::IsTerminator() const
{
    return true;
}
