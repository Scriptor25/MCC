#include <mcc/value.hpp>

mcc::ValuePtr mcc::BranchResult::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ResourceLocation &location)
{
    auto self  = std::make_shared<BranchResult>(where, name, type, location);
    self->Self = self;
    return self;
}

mcc::BranchResult::BranchResult(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        ResourceLocation location)
    : Value(where,
            name,
            type,
            FieldType_::ImmutableReference),
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
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Location.String(),
        .Path          = std::format("stack[0].x{}", StackId),
    };
}
