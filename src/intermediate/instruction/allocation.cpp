#include <mcc/error.hpp>
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

void mcc::AllocationInstruction::Generate(const Builder &builder, CommandVector &commands, const bool use_stack) const
{
    Assert(use_stack, "allocation instruction requires stack usage");

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

mcc::Result mcc::AllocationInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    Assert(use_stack, "allocation instruction requires stack usage");

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = std::format("stack[0].val[{}]", Index),
    };
}

bool mcc::AllocationInstruction::RequireStack() const
{
    return true;
}
