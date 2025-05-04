#include <mcc/intermediate.hpp>

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

mcc::ConstantFloatRange::ConstantFloatRange(std::optional<FloatT> min, std::optional<FloatT> max)
    : Min(std::move(min)),
      Max(std::move(max))
{
}

mcc::Result mcc::ConstantFloatRange::GenResult(const bool stringify, bool use_stack) const
{
    if (Min == Max)
    {
        auto value = std::to_string(Min.value());
        if (stringify)
            value += '"' + value + '"';

        return {
            .Type = ResultType_Value,
            .Value = value,
        };
    }

    const auto min_string = Min.has_value()
                                ? std::to_string(Min.value())
                                : "";

    const auto max_string = Max.has_value()
                                ? std::to_string(Max.value())
                                : "";

    auto value = min_string + ".." + max_string;
    if (stringify)
        value += '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = value,
    };
}
