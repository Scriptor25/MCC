#include <utility>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ThrowInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &value,
    const BlockPtr &landing_pad)
{
    return std::make_shared<ThrowInstruction>(
        where,
        context,
        location,
        value,
        landing_pad);
}

mcc::ThrowInstruction::ThrowInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr value,
    BlockPtr landing_pad)
    : Instruction(where, context.GetVoid(), false),
      Location(std::move(location)),
      Value(std::move(value)),
      LandingPad(std::move(landing_pad))
{
    Value->Use();
    if (LandingPad)
    {
        LandingPad->Use();
    }
}

mcc::ThrowInstruction::~ThrowInstruction()
{
    Value->Drop();
    if (LandingPad)
    {
        LandingPad->Drop();
    }
}

void mcc::ThrowInstruction::Generate(CommandVector &commands, const bool stack) const
{
    switch (auto value = Value->GenerateResult(false); value.Type)
    {
        case ResultType_Value:
            commands.Append("data modify storage {} result set value {}", Location, value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} result set from storage {} {}",
                Location,
                value.Location,
                value.Path);
            break;

        case ResultType_Argument:
            commands.Append("$data modify storage {} result set value $({})", Location, value.Name);
            break;

        default:
            Error(
                Where,
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Argument,
                value.Type);
    }

    if (LandingPad)
    {
        std::string prefix, arguments;
        LandingPad->Parent->ForwardArguments(prefix, arguments);

        commands.Append("{}return run function {}{}", prefix, LandingPad->Parent->GetLocation(LandingPad), arguments);
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
