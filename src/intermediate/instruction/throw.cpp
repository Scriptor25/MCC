#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ThrowInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const ValuePtr &value,
        const BlockPtr &landing_pad)
{
    auto self = std::make_shared<ThrowInstruction>(where, name, context, location, value, landing_pad);

    self->Self = self;
    self->Value->Use(self);
    if (self->LandingPad)
        self->LandingPad->Use(self);

    return self;
}

mcc::ThrowInstruction::ThrowInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        ResourceLocation location,
        ValuePtr value,
        BlockPtr landing_pad)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Location(std::move(location)),
      Value(std::move(value)),
      LandingPad(std::move(landing_pad))
{
}

mcc::ThrowInstruction::~ThrowInstruction()
{
    Value->Drop(Self);
    if (LandingPad)
        LandingPad->Drop(Self);
}

void mcc::ThrowInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    auto value = Value->GenerateResult();

    std::string value_prefix;
    if (value.WithArgument)
        value_prefix = "$";

    switch (value.Type)
    {
    case ResultType_::Value:
        commands.Append("{}data modify storage {} result set value {}", value_prefix, Location, value.Value);
        break;

    case ResultType_::Reference:
        commands.Append(
                "{}data modify storage {} result set from {} {} {}",
                value_prefix,
                Location,
                value.ReferenceType,
                value.Target,
                value.Path);
        break;

    case ResultType_::Argument:
        commands.Append("$data modify storage {} result set value {}", Location, value.Name);
        break;

    default:
        Error(Where,
              "value must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
              value.Type);
    }

    if (LandingPad)
    {
        std::string prefix, arguments;
        LandingPad->Parent->ForwardArguments(prefix, arguments);

        commands.Append("{}return run function {}{}", prefix, LandingPad->GetLocation(), arguments);
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
