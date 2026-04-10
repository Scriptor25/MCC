#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ObjectInstruction::CreateInsert(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const ValuePtr &object,
        const ValuePtr &value,
        const std::string &key)
{
    auto self = std::make_shared<ObjectInstruction>(where, name, context, object, value, key);

    self->Self = self;
    self->Object->Use(self);
    self->Value->Use(self);

    return self;
}

mcc::ObjectInstruction::ObjectInstruction(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        ValuePtr object,
        ValuePtr value,
        std::string key)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Object(std::move(object)),
      Value(std::move(value)),
      Key(std::move(key))
{
}

mcc::ObjectInstruction::~ObjectInstruction()
{
    Object->Drop(Self);
    Value->Drop(Self);
}

void mcc::ObjectInstruction::Generate(
        CommandVector &commands,
        bool stack) const
{
    auto object = Object->GenerateResult();
    auto value  = Value->GenerateResult();

    std::string prefix;
    if (object.WithArgument || value.WithArgument)
        prefix = "$";

    switch (value.Type)
    {
    case ResultType_::Value:
        commands.Append(
                "{}data modify {} {} {}.{} set value {}",
                prefix,
                object.ReferenceType,
                object.Target,
                object.Path,
                Key,
                value.Value);
        break;

    case ResultType_::Reference:
        commands.Append(
                "{}data modify {} {} {}.{} set from {} {} {}",
                prefix,
                object.ReferenceType,
                object.Target,
                object.Path,
                Key,
                value.ReferenceType,
                value.Target,
                value.Path);
        break;

    case ResultType_::Argument:
        commands.Append(
                "$data modify {} {} {}.{} set value {}",
                object.ReferenceType,
                object.Target,
                object.Path,
                Key,
                value.Name);
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

bool mcc::ObjectInstruction::RequireStack() const
{
    return Object->RequireStack() || Value->RequireStack();
}
