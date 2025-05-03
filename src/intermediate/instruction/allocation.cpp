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
    // data modify storage <location> stack[0].values insert <index> value (value: 0|array: []|object: {})
}

void mcc::AllocationInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::AllocationInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
