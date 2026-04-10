#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent,
        const ValuePtr &value)
{
    auto self = std::make_shared<ReturnInstruction>(where, name, context, parent, value);

    self->Self = self;
    if (self->Value)
        self->Value->Use(self);

    return self;
}

mcc::ReturnInstruction::ReturnInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent,
        ValuePtr value)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Parent(std::move(parent)),
      Value(std::move(value))
{
}

mcc::ReturnInstruction::~ReturnInstruction()
{
    if (Value)
        Value->Drop(Self);
}

void mcc::ReturnInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    auto location = Parent->Mangle();

    if (Value)
    {
        auto value = Value->GenerateResult();

        std::string prefix;
        if (value.WithArgument)
            prefix = "$";

        switch (value.Type)
        {
        case ResultType_::Value:
            commands.Append("{}data modify storage {} result set value {}", prefix, location, value.Value);
            break;

        case ResultType_::Reference:
            commands.Append(
                    "{}data modify storage {} result set from {} {} {}",
                    prefix,
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
    }
    else
        commands.Append("data remove storage {} result", location);

    if (stack)
        commands.Append("data remove storage {} stack[0]", location);

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
