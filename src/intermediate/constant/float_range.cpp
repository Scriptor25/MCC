#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantFloatRange::Create(
    const SourceLocation &where,
    TypeContext &context,
    const FloatT min,
    const FloatT max)
{
    return std::make_shared<ConstantFloatRange>(where, context, min, max);
}

mcc::ConstantPtr mcc::ConstantFloatRange::CreateMin(
    const SourceLocation &where,
    TypeContext &context,
    const FloatT min)
{
    return std::make_shared<ConstantFloatRange>(where, context, min, std::nullopt);
}

mcc::ConstantPtr mcc::ConstantFloatRange::CreateMax(
    const SourceLocation &where,
    TypeContext &context,
    const FloatT max)
{
    return std::make_shared<ConstantFloatRange>(where, context, std::nullopt, max);
}

mcc::ConstantFloatRange::ConstantFloatRange(
    const SourceLocation &where,
    TypeContext &context,
    const std::optional<FloatT> min,
    const std::optional<FloatT> max)
    : Constant(where, context, context.GetNull()),
      Min(min),
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
