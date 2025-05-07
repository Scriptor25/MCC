#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(ResourceLocation location, ConstantPtr nbt)
{
    return std::make_shared<ConstantResource>(std::move(location), std::move(nbt));
}

mcc::ConstantResource::ConstantResource(ResourceLocation location, ConstantPtr nbt)
    : Location(std::move(location)),
      NBT(std::move(nbt))
{
}

mcc::Result mcc::ConstantResource::GenerateResult(const bool stringify, const bool use_stack) const
{
    auto value = Location.String();

    if (NBT)
    {
        auto nbt = NBT->GenerateResult(stringify, use_stack);
        Assert(nbt.Type == ResultType_Value, "nbt must be {}, but is {}", ResultType_Value, nbt.Type);
        value += nbt.Value;
    }

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
