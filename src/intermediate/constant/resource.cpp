#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(ResourceLocation location, ConstantPtr state, ConstantPtr data)
{
    return std::make_shared<ConstantResource>(std::move(location), std::move(state), std::move(data));
}

mcc::ConstantResource::ConstantResource(ResourceLocation location, ConstantPtr state, ConstantPtr data)
    : Location(std::move(location)),
      State(std::move(state)),
      Data(std::move(data))
{
}

mcc::Result mcc::ConstantResource::GenerateResult(const bool stringify) const
{
    auto value = Location.String();

    if (State)
    {
        auto state = State->GenerateResult(stringify);
        Assert(state.Type == ResultType_Value, "state must be {}, but is {}", ResultType_Value, state.Type);
        value += state.Value;
    }

    if (Data)
    {
        auto data = Data->GenerateResult(stringify);
        Assert(data.Type == ResultType_Value, "data must be {}, but is {}", ResultType_Value, data.Type);
        value += data.Value;
    }

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
