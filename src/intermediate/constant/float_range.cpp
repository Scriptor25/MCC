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

mcc::Command mcc::ConstantFloatRange::GenInline() const
{
    if (Min == Max)
        return std::to_string(Min.value());

    const auto min_string = Min.has_value()
                                ? std::to_string(Min.value())
                                : "";

    const auto max_string = Max.has_value()
                                ? std::to_string(Max.value())
                                : "";

    return min_string + ".." + max_string;
}
