#include <mcc/value.hpp>
#include <utility>

mcc::ValuePtr mcc::BranchResult::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location)
{
    return std::make_shared<BranchResult>(where, type, location);
}

mcc::BranchResult::BranchResult(const SourceLocation &where, const TypePtr &type, ResourceLocation location)
    : Value(where, type, false),
      Location(std::move(location))
{
}

bool mcc::BranchResult::RequireStack() const
{
    return true;
}

mcc::Result mcc::BranchResult::GenerateResult(bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = std::format("stack[0].result.{}", reinterpret_cast<uintptr_t>(this)),
    };
}
