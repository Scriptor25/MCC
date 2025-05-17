#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::ThrowInstruction::Create(ResourceLocation location, ValuePtr value, const BlockPtr landing_pad)
{
    return std::make_shared<ThrowInstruction>(std::move(location), std::move(value), std::move(landing_pad));
}

mcc::ThrowInstruction::ThrowInstruction(ResourceLocation location, ValuePtr value, const BlockPtr landing_pad)
    : Location(std::move(location)),
      Value(std::move(value)),
      LandingPad(std::move(landing_pad))
{
    Value->Use();
}

mcc::ThrowInstruction::~ThrowInstruction()
{
    Value->Drop();
}

void mcc::ThrowInstruction::Generate(CommandVector &commands, const bool stack) const
{
    switch (auto value = Value->GenerateResult(false); value.Type)
    {
        case ResultType_Value:
            commands.Append(
                "data modify storage {} result set value {}",
                Location,
                value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} result set from storage {} {}",
                Location,
                value.Location,
                value.Path);
            break;

        case ResultType_Score:
            commands.Append(
                "execute store result storage {} result double 1 run scoreboard players get {} {}",
                Location,
                value.Player,
                value.Objective);
            break;

        default:
            Error(
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                value.Type);
    }

    if (LandingPad)
    {
        auto landing_pad = LandingPad->Location;

        std::string prefix, arguments;
        if (auto &parameters = LandingPad->Parent->Parameters; !parameters.empty())
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

        commands.Append("{}return run function {}{}", prefix, landing_pad, arguments);
        return;
    }

    if (stack)
        commands.Append("data remove storage {} stack[0]", Location);

    commands.Append("return 1");
}

bool mcc::ThrowInstruction::RequireStack() const
{
    return Value->RequireStack();
}

bool mcc::ThrowInstruction::IsTerminator() const
{
    return true;
}
