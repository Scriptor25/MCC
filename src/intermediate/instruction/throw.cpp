#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ThrowInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const ValuePtr &value,
        const BlockPtr &landing_pad)
{
    auto self = std::make_shared<ThrowInstruction>(where, name, context, parent, value, landing_pad);

    self->Self = self;
    self->Value->Use(self);
    if (self->LandingPad)
        self->LandingPad->Use(self);

    return self;
}

mcc::ThrowInstruction::ThrowInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent,
        ValuePtr value,
        BlockPtr landing_pad)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Parent(std::move(parent)),
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
    auto location = Parent->Mangle();

    auto value = Value->GenerateResult();

    std::string value_prefix;
    if (value.WithArgument)
        value_prefix = "$";

    switch (value.Type)
    {
    case ResultType_::Value:
        commands.Append("{}data modify storage {} result set value {}", value_prefix, location, value.Value);
        break;

    case ResultType_::Reference:
        commands.Append(
                "{}data modify storage {} result set from {} {} {}",
                value_prefix,
                location,
                value.ReferenceType,
                value.Target,
                value.Path);
        break;

    case ResultType_::Argument:
        commands.Append("$data modify storage {} result set value {}", location, value.Name);
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
        Parent->ForwardArguments(prefix, arguments);

        commands.Append("{}return run function {}{}", prefix, LandingPad->GetLocation(), arguments);
        return;
    }

    if (stack)
        commands.Append("data remove storage {} stack[0]", location);

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
