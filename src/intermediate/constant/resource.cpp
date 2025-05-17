#include <mcc/error.hpp>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ConstantPtr &state,
    const ConstantPtr &data)
{
    return std::make_shared<ConstantResource>(where, location, state, data);
}

mcc::ConstantResource::ConstantResource(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ConstantPtr &state,
    const ConstantPtr &data)
    : Constant(where, TypeContext::GetNull()),
      Location(location),
      State(state),
      Data(data)
{
}

mcc::Result mcc::ConstantResource::GenerateResult(const bool stringify) const
{
    auto value = Location.String();

    if (State)
    {
        auto state = State->GenerateResult(stringify);
        Assert(state.Type == ResultType_Value, Where, "state must be {}, but is {}", ResultType_Value, state.Type);
        value += state.Value;
    }

    if (Data)
    {
        auto data = Data->GenerateResult(stringify);
        Assert(data.Type == ResultType_Value, Where, "data must be {}, but is {}", ResultType_Value, data.Type);
        value += data.Value;
    }

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
