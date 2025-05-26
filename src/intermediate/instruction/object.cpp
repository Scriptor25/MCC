#include <utility>
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
    ResourceLocation location,
    ValuePtr object,
    ValuePtr value,
    std::string key)
    : Instruction(where, context.GetVoid(), false),
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

    switch (auto value = Value->GenerateResult(); value.Type)
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

        case ResultType_Argument:
            commands.Append(
                "$data modify storage {} {}.{} set value {}",
                object.Location,
                object.Path,
                Key,
                value.Name);
            break;

        default:
            Error(
                Where,
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                value.Type);
    }
}

bool mcc::ObjectInstruction::RequireStack() const
{
    return Object->RequireStack() || Value->RequireStack();
}
