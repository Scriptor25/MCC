#include <utility>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::AllocationInstruction::CreateValue(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location,
    const IndexT index)
{
    return std::make_shared<AllocationInstruction>(where, context, type, AllocationType_Value, location, index);
}

mcc::InstructionPtr mcc::AllocationInstruction::CreateArray(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location,
    const IndexT index)
{
    return std::make_shared<AllocationInstruction>(where, context, type, AllocationType_Array, location, index);
}

mcc::InstructionPtr mcc::AllocationInstruction::CreateObject(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location,
    const IndexT index)
{
    return std::make_shared<AllocationInstruction>(where, context, type, AllocationType_Object, location, index);
}

mcc::AllocationInstruction::AllocationInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const AllocationTypeE allocation_type,
    ResourceLocation location,
    const IndexT index)
    : Instruction(where, context, type),
      AllocationType(allocation_type),
      Location(std::move(location)),
      Index(index)
{
}

void mcc::AllocationInstruction::Generate(CommandVector &commands, const bool stack) const
{
    std::string value;
    switch (AllocationType)
    {
        case AllocationType_Value:
            value = "0";
            break;

        case AllocationType_Array:
            value = "[]";
            break;

        case AllocationType_Object:
            value = "{}";
            break;
    }

    Assert(stack, Where, "allocation instruction requires stack");
    commands.Append("data modify storage {} stack[0].val[{}] set value {}", Location, Index, value);
}

bool mcc::AllocationInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::AllocationInstruction::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = std::format("stack[0].val[{}]", Index),
    };
}
