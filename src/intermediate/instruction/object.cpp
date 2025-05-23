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
    return std::make_shared<ObjectInstruction>(
        where,
        context,
        location,
        object,
        value,
        key);
}

mcc::ObjectInstruction::ObjectInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &object,
    const ValuePtr &value,
    const std::string &key)
    : Instruction(where, context, context.GetVoid()),
      Location(location),
      Object(object),
      Value(value),
      Key(key)
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
    auto object = Object->GenerateResult(false);
    auto value = Value->GenerateResult(false);

    switch (value.Type)
    {
        case ResultType_Value:
            commands.Append(
                "data modify storage {} {}.{} set value {}",
                object.Location,
                object.Path,
                Key,
                value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} {}.{} set from storage {} {}",
                object.Location,
                object.Path,
                Key,
                value.Location,
                value.Path);
            break;

        default:
            Error(
                Where,
                "value must be {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                value.Type);
    }
}

bool mcc::ObjectInstruction::RequireStack() const
{
    return Object->RequireStack() || Value->RequireStack();
}
