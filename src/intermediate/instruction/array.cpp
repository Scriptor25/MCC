#include <utility>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value)
{
    return std::make_shared<ArrayInstruction>(where, context, ArrayOperation_Append, location, array, value, ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value)
{
    return std::make_shared<ArrayInstruction>(where, context, ArrayOperation_Prepend, location, array, value, ~0);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const IndexT index)
{
    return std::make_shared<ArrayInstruction>(where, context, ArrayOperation_Insert, location, array, value, index);
}

mcc::ArrayInstruction::ArrayInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const E_ArrayOperation array_operation,
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const IndexT index)
    : Instruction(where, context.GetVoid(), false),
      ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index)
{
    Array->Use();
    if (Value)
        Value->Use();
}

mcc::ArrayInstruction::~ArrayInstruction()
{
    Array->Drop();
    if (Value)
        Value->Drop();
}

void mcc::ArrayInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto array = Array->GenerateResult();
    auto value = Value->GenerateResult();

    Assert(array.Type == ResultType_Reference, Where, "array must be {}, but is {}", ResultType_Reference, array.Type);

    std::string operation;
    switch (ArrayOperation)
    {
    case ArrayOperation_Append:
        operation = "append";
        break;

    case ArrayOperation_Prepend:
        operation = "prepend";
        break;

    case ArrayOperation_Insert:
        operation = std::format("insert {}", Index);
        break;
    }

    std::string prefix;
    if (value.WithArgument)
        prefix = "$";

    switch (value.Type)
    {
    case ResultType_Value:
        commands.Append(
            "{}data modify {} {} {} {} value {}",
            prefix,
            array.ReferenceType,
            array.Target,
            array.Path,
            operation,
            value.Value);
        break;

    case ResultType_Reference:
        commands.Append(
            "{}data modify {} {} {} {} from {} {} {}",
            prefix,
            array.ReferenceType,
            array.Target,
            array.Path,
            operation,
            value.ReferenceType,
            value.Target,
            value.Path);
        break;

    case ResultType_Argument:
        commands
                .Append(
                    "$data modify {} {} {} {} value {}",
                    array.ReferenceType,
                    array.Target,
                    array.Path,
                    operation,
                    value.Name);
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

bool mcc::ArrayInstruction::RequireStack() const
{
    return Array->RequireStack() || Value->RequireStack();
}
