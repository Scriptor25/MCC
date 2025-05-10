#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::DirectInstruction::Create(ResourceLocation location, BlockPtr target)
{
    return std::make_shared<DirectInstruction>(std::move(location), std::move(target), nullptr, nullptr);
}

mcc::InstructionPtr mcc::DirectInstruction::Create(
    ResourceLocation location,
    BlockPtr target,
    ValuePtr result,
    ValuePtr landing_pad)
{
    return std::make_shared<DirectInstruction>(
        std::move(location),
        std::move(target),
        std::move(result),
        std::move(landing_pad));
}

mcc::DirectInstruction::DirectInstruction(
    ResourceLocation location,
    BlockPtr target,
    ValuePtr result,
    ValuePtr landing_pad)
    : Location(std::move(location)),
      Target(std::move(target)),
      Result(std::move(result)),
      LandingPad(std::move(landing_pad))
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
        auto landing_pad = LandingPad->GenerateResult(false);

        Assert(
            landing_pad.Type == ResultType_Storage,
            "landing pad must be {}, but is {}",
            ResultType_Storage,
            landing_pad.Type);

        switch (result.Type)
        {
            case ResultType_Value:
                commands.Append(
                    "data modify storage {} {} set value {}",
                    landing_pad.Location,
                    landing_pad.Path,
                    result.Value);
                break;

            case ResultType_Storage:
                commands.Append(
                    "data modify storage {} {} set from storage {} {}",
                    landing_pad.Location,
                    landing_pad.Path,
                    result.Location,
                    result.Path);
                break;

            case ResultType_Score:
                commands.Append(
                    "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                    landing_pad.Location,
                    landing_pad.Path,
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
    return Result ? Result->RequireStack() || LandingPad->RequireStack() : false;
}

bool mcc::DirectInstruction::IsTerminator() const
{
    return true;
}
