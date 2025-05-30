#include <utility>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::AllocationInstruction::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const IndexT index,
    const bool is_mutable,
    const ConstantPtr &initializer)
{
    return std::make_shared<AllocationInstruction>(where, type, location, index, is_mutable, initializer);
}

mcc::AllocationInstruction::AllocationInstruction(
    const SourceLocation &where,
    const TypePtr &type,
    ResourceLocation location,
    const IndexT index,
    const bool is_mutable,
    ConstantPtr initializer)
    : Instruction(where, type, is_mutable),
      Location(std::move(location)),
      Index(index),
      Initializer(std::move(initializer))
{
}

void mcc::AllocationInstruction::Generate(CommandVector &commands, const bool stack) const
{
    Assert(stack, Where, "allocation instruction requires stack");

    const auto initializer = Initializer ? Initializer : Type->GetNull(Where);

    if (!initializer)
        return;

    auto initializer_value = initializer->GenerateResult();
    Assert(
        initializer_value.Type == ResultType_Value,
        Where,
        "initializer must be {}, but is {}",
        ResultType_Value,
        initializer_value.Type);

    commands.Append("data modify storage {} stack[0].val[{}] set value {}", Location, Index, initializer_value.Value);
}

bool mcc::AllocationInstruction::RequireStack() const
{
    return true;
}

mcc::Result mcc::AllocationInstruction::GenerateResult() const
{
    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = Location.String(),
        .Path = std::format("stack[0].val[{}]", Index),
    };
}
