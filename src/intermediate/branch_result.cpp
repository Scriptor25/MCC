#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
        const SourceLocation &where,
        const TypePtr &type,
        const ResourceLocation &location)
{
    auto self  = std::make_shared<BranchResult>(where, type, location);
    self->Self = self;
    return self;
}

mcc::BranchResult::BranchResult(
        const SourceLocation &where,
        const TypePtr &type,
        ResourceLocation location)
    : Value(where,
            type,
            FieldType_ImmutableReference),
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
        .Type          = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target        = Location.String(),
        .Path          = std::format("stack[0].x{}", StackId),
    };
}
