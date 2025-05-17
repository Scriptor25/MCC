#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(ResourceLocation location)
{
    return std::make_shared<BranchResult>(std::move(location));
}

mcc::BranchResult::BranchResult(ResourceLocation location)
    : Location(std::move(location))
{
}

bool mcc::BranchResult::RequireStack() const
{
    return true;
}

mcc::Result mcc::BranchResult::GenerateResult(bool stringify) const
{
    auto stack_path = std::format("stack[0].result.{}", reinterpret_cast<uintptr_t>(this));

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = std::move(stack_path),
    };
}
