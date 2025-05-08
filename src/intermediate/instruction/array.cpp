#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ArrayInstruction::CreateAppend(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Append,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreatePrepend(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Prepend,
        std::move(location),
        std::move(array),
        std::move(value),
        ~0,
        stringify);
}

mcc::InstructionPtr mcc::ArrayInstruction::CreateInsert(
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    IndexT index,
    bool stringify)
{
    return std::make_shared<ArrayInstruction>(
        ArrayOperation_Insert,
        std::move(location),
        std::move(array),
        std::move(value),
        index,
        stringify);
}

mcc::ArrayInstruction::ArrayInstruction(
    const ArrayOperationE array_operation,
    ResourceLocation location,
    ValuePtr array,
    ValuePtr value,
    const IndexT index,
    const bool stringify)
    : ArrayOperation(array_operation),
      Location(std::move(location)),
      Array(std::move(array)),
      Value(std::move(value)),
      Index(index),
      Stringify(stringify)
{
    Array->Use();
    Value->Use();
}

mcc::ArrayInstruction::~ArrayInstruction()
{
    Array->Drop();
    Value->Drop();
}

void mcc::ArrayInstruction::Generate(CommandVector &commands) const
{
    //
    // array: stack-allocated
    //
    //      data modify storage <location> stack[0].values[<array.index>] (append|prepend|insert <index>) ...
    //
    // value -> constant:           ... value <value>
    //       -> stack-allocated:    ... from storage <location> stack[0].values[<value.index>]
    //
    // value -> register:
    //
    //      data modify storage <location> stack[0].values[<array.index>] (append|prepend|insert <index>) value 0
    //      execute store result storage <location> stack[0].values[<array.index>][(<back>|0|<index>)] double 1 run scoreboard players get <value.player> <value.objective>
    //

    auto array = Array->GenerateResult(false);
    auto value = Value->GenerateResult(Stringify);

    Assert(
        array.Type == ResultType_Storage,
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
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                value.Type);
    }
}
