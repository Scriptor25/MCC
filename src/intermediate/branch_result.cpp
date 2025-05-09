#include <mcc/intermediate.hpp>

mcc::ValuePtr mcc::BranchResult::Create(ResourceLocation location)
{
    return std::make_shared<BranchResult>(std::move(location));
}

mcc::BranchResult::BranchResult(ResourceLocation location)
    : Location(std::move(location))
{
}

mcc::Result mcc::BranchResult::GenerateResult(bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "result",
    };
}
