#include <utility>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ObjectInstruction::CreateInsert(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key)
{
    return std::make_shared<ObjectInstruction>(where, context, location, object, value, key);
}

mcc::ObjectInstruction::ObjectInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr object,
    ValuePtr value,
    std::string key)
    : Instruction(where, context.GetVoid(), FieldType_Value),
      Location(std::move(location)),
      Object(std::move(object)),
      Value(std::move(value)),
      Key(std::move(key))
{
    Object->Use();
    Value->Use();
}

mcc::ObjectInstruction::~ObjectInstruction()
{
    Object->Drop();
    Value->Drop();
}

void mcc::ObjectInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto object = Object->GenerateResult();
    auto value = Value->GenerateResult();

    std::string prefix;
    if (object.WithArgument || value.WithArgument)
        prefix = "$";

    switch (value.Type)
    {
    case ResultType_Value:
        commands.Append(
            "{}data modify {} {} {}.{} set value {}",
            prefix,
            object.ReferenceType,
            object.Target,
            object.Path,
            Key,
            value.Value);
        break;

    case ResultType_Reference:
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

    case ResultType_Argument:
        commands.Append(
            "$data modify {} {} {}.{} set value {}",
            object.ReferenceType,
            object.Target,
            object.Path,
            Key,
            value.Name);
        break;

    default:
        Error(Where, "value must be {}, {} or {}, but is {}", ResultType_Value, ResultType_Reference, value.Type);
    }
}

bool mcc::ObjectInstruction::RequireStack() const
{
    return Object->RequireStack() || Value->RequireStack();
}
