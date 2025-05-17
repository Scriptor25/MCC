#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

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

    Assert(stack, "allocation instruction requires stack");
    commands.Append("data modify storage {} stack[0].val insert {} value {}", Location, Index, value);
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
