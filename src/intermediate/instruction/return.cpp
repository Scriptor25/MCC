#include <utility>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &value)
{
    return std::make_shared<ReturnInstruction>(where, context, location, value);
}

mcc::ReturnInstruction::ReturnInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr value)
    : Instruction(where, context.GetVoid(), false),
      Location(std::move(location)),
      Value(std::move(value))
{
    if (Value)
        Value->Use();
}

mcc::ReturnInstruction::~ReturnInstruction()
{
    if (Value)
        Value->Drop();
}

void mcc::ReturnInstruction::Generate(CommandVector &commands, const bool stack) const
{
    if (Value)
    {
        auto value = Value->GenerateResult();

        std::string prefix;
        if (value.WithArgument)
            prefix = "$";

        switch (value.Type)
        {
        case ResultType_Value:
            commands.Append("{}data modify storage {} result set value {}", prefix, Location, value.Value);
            break;

        case ResultType_Reference:
            commands.Append(
                "{}data modify storage {} result set from {} {} {}",
                prefix,
                Location,
                value.ReferenceType,
                value.Target,
                value.Path);
            break;

        case ResultType_Argument:
            commands.Append("$data modify storage {} result set value {}", Location, value.Name);
            break;

        default:
            Error(
                Where,
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Reference,
                ResultType_Argument,
                value.Type);
        }
    }
    else
        commands.Append("data remove storage {} result", Location);

    if (stack)
        commands.Append("data remove storage {} stack[0]", Location);

    commands.Append("return 0");
}

bool mcc::ReturnInstruction::RequireStack() const
{
    return Value && Value->RequireStack();
}

bool mcc::ReturnInstruction::IsTerminator() const
{
    return true;
}
