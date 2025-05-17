#include <mcc/constant.hpp>

mcc::ConstantPtr mcc::ConstantFloatRange::Create(FloatT min, FloatT max)
{
    return std::make_shared<ConstantFloatRange>(min, max);
}

mcc::ConstantPtr mcc::ConstantFloatRange::CreateMin(FloatT min)
{
    return std::make_shared<ConstantFloatRange>(min, std::nullopt);
}

mcc::ConstantPtr mcc::ConstantFloatRange::CreateMax(FloatT max)
{
    return std::make_shared<ConstantFloatRange>(std::nullopt, max);
}

mcc::ConstantFloatRange::ConstantFloatRange(const std::optional<FloatT> min, const std::optional<FloatT> max)
    : Min(min),
      Max(max)
{
}

mcc::Result mcc::ConstantFloatRange::GenerateResult(const bool stringify) const
{
    if (Min == Max)
    {
        auto value = std::to_string(Min.value());
        if (stringify)
            value = '"' + value + '"';

        return {
            .Type = ResultType_Value,
            .Value = std::move(value),
        };
    }

    std::string value;

    value += Min.has_value()
                 ? std::to_string(Min.value())
                 : "";
    value += "..";
    value += Max.has_value()
                 ? std::to_string(Max.value())
                 : "";

    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
