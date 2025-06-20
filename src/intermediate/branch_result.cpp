#include <utility>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location)
{
    return std::make_shared<BranchResult>(where, type, location);
}

mcc::BranchResult::BranchResult(const SourceLocation &where, const TypePtr &type, ResourceLocation location)
    : Value(where, type, FieldType_ImmutableReference),
      Location(std::move(location))
{
}

bool mcc::BranchResult::RequireStack() const
{
    return true;
}

mcc::Result mcc::BranchResult::GenerateResult() const
{
    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = Location.String(),
        .Path = std::format("stack[0].{}", reinterpret_cast<uintptr_t>(this)),
    };
}
