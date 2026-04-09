#include <mcc/value.hpp>

mcc::ValuePtr mcc::FunctionResult::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ResourceLocation &location)
{
    auto self  = std::make_shared<FunctionResult>(where, name, type, location);
    self->Self = self;
    return self;
}

mcc::FunctionResult::FunctionResult(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        ResourceLocation location)
    : Value(where,
            name,
            type,
            FieldType_ImmutableReference),
      Location(std::move(location))
{
}

bool mcc::FunctionResult::RequireStack() const
{
    return false;
}

mcc::Result mcc::FunctionResult::GenerateResult() const
{
    return {
        .Type          = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target        = Location.String(),
        .Path          = "result",
    };
}
