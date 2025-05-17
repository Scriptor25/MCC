#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
    const SourceLocation &where,
    TypePtr type,
    const ResourceLocation &location)
{
    return std::make_shared<BranchResult>(where, type, location);
}

mcc::BranchResult::BranchResult(const SourceLocation &where, TypePtr type, const ResourceLocation &location)
    : Value(where, type),
      Location(location)
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
