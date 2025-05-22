#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location)
{
    return std::make_shared<BranchResult>(where, context, type, location);
}

mcc::BranchResult::BranchResult(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location)
    : Value(where, context, type),
      Location(location)
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
