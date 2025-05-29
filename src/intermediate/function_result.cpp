#include <mcc/value.hpp>
#include <utility>

mcc::ValuePtr mcc::FunctionResult::Create(const SourceLocation &where, const TypePtr &type, const ResourceLocation &location)
{
    return std::make_shared<FunctionResult>(where, type, location);
}

mcc::FunctionResult::FunctionResult(const SourceLocation &where, const TypePtr &type, ResourceLocation location)
    : Value(where, type, false), Location(std::move(location))
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
