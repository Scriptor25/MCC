#include <mcc/value.hpp>

mcc::ValuePtr mcc::FunctionResult::Create(const SourceLocation &where, const ResourceLocation &location)
{
    return std::make_shared<FunctionResult>(where, location);
}

mcc::FunctionResult::FunctionResult(const SourceLocation &where, const ResourceLocation &location)
    : Value(where),
      Location(location)
{
}

bool mcc::FunctionResult::RequireStack() const
{
    return false;
}

mcc::Result mcc::FunctionResult::GenerateResult(bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "result",
    };
}
