#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <utility>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        context,
        context.GetVoid(),
        ArrayOperation_Append,
        location,
        array,
        value,
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        context,
        context.GetVoid(),
        ArrayOperation_Prepend,
        location,
        array,
        value,
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const IndexT index,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        context,
        context.GetVoid(),
        ArrayOperation_Insert,
        location,
        array,
        value,
        index,
        stringify);
}

mcc::ArrayInstruction::ArrayInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ArrayOperationE array_operation,
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const IndexT index,
    const bool stringify)
    : Instruction(where, context, type),
      ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index),
      Stringify(stringify)
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
    auto array = Array->GenerateResult(false);
    auto value = Value->GenerateResult(Stringify);

    Assert(
        array.Type == ResultType_Storage,
        Where,
        "array must be {}, but is {}",
        ResultType_Storage,
        array.Type);

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

    auto conversion = Stringify ? "string" : "from";

    switch (value.Type)
    {
        case ResultType_Value:
            commands.Append(
                "data modify storage {} {} {} value {}",
                array.Location,
                array.Path,
                operation,
                value.Value);
            break;

        case ResultType_Storage:
            commands.Append(
                "data modify storage {} {} {} {} storage {} {}",
                array.Location,
                array.Path,
                operation,
                conversion,
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

bool mcc::ArrayInstruction::RequireStack() const
{
    return Array->RequireStack() || Value->RequireStack();
}
