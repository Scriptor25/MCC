#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        TypeContext::GetVoid(),
        ArrayOperation_Append,
        location,
        array,
        value,
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        TypeContext::GetVoid(),
        ArrayOperation_Prepend,
        location,
        array,
        value,
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const IndexT index,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        where,
        TypeContext::GetVoid(),
        ArrayOperation_Insert,
        location,
        array,
        value,
        index,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateExtract(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ValuePtr &array,
    const IndexT index)
{
    auto type = std::dynamic_pointer_cast<ArrayType>(array->Type)->Elements;

    return std::make_shared<ArrayInstruction>(
        where,
        type,
        ArrayOperation_Extract,
        location,
        array,
        nullptr,
        index,
        false);
}

mcc::ArrayInstruction::ArrayInstruction(
    const SourceLocation &where,
    const TypePtr &type,
    const ArrayOperationE array_operation,
    const ResourceLocation &location,
    const ValuePtr &array,
    const ValuePtr &value,
    const IndexT index,
    const bool stringify)
    : Instruction(where, type),
      ArrayOperation(array_operation),
      Location(location),
      Array(array),
      Value(value),
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
    if (ArrayOperation == ArrayOperation_Extract)
        return;

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

        default:
            Error(Where, "TODO");
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

        case ResultType_Score:
            commands.Append(
                "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                Location,
                GetTmpName(),
                value.Player,
                value.Objective);
            commands.Append(
                "data modify storage {} {} {} {} storage {} {}",
                array.Location,
                array.Path,
                operation,
                conversion,
                Location,
                GetTmpName());
            commands.Append("data remove storage {} {}", Location, GetTmpName());
            break;

        default:
            Error(
                Where,
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                value.Type);
    }
}

bool mcc::ArrayInstruction::RequireStack() const
{
    return Array->RequireStack() || Value->RequireStack();
}

mcc::Result mcc::ArrayInstruction::GenerateResult(bool stringify) const
{
    Assert(ArrayOperation == ArrayOperation_Extract, Where, "only the extract array operation produces a result");

    auto array = Array->GenerateResult(false);

    Assert(
        array.Type == ResultType_Storage,
        Where,
        "array must be {}, but is {}",
        ResultType_Storage,
        array.Type);

    return {
        .Type = ResultType_Storage,
        .Location = array.Location,
        .Path = std::format("{}[{}]", array.Path, Index),
    };
}
