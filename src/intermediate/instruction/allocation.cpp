#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::AllocationInstruction::CreateValue(ResourceLocation location, IndexT index)
{
    return std::make_shared<AllocationInstruction>(AllocationType_Value, std::move(location), index);
}

mcc::InstructionPtr mcc::AllocationInstruction::CreateArray(ResourceLocation location, IndexT index)
{
    return std::make_shared<AllocationInstruction>(AllocationType_Array, std::move(location), index);
}

mcc::InstructionPtr mcc::AllocationInstruction::CreateObject(ResourceLocation location, IndexT index)
{
    return std::make_shared<AllocationInstruction>(AllocationType_Object, std::move(location), index);
}

mcc::AllocationInstruction::AllocationInstruction(
    const AllocationTypeE allocation_type,
    ResourceLocation location,
    const IndexT index)
    : AllocationType(allocation_type),
      Location(std::move(location)),
      Index(index)
{
}

void mcc::AllocationInstruction::Gen(CommandVector &commands) const
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

    commands.Append("data modify storage {} stack[0].val insert {} value {}", Location, Index, value);
}

mcc::CommandResult mcc::AllocationInstruction::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].val[" + std::to_string(Index) + ']',
    };
}
