#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantTarget::Create(
    TargetSelectorE selector,
    std::map<std::string, std::vector<ConstantPtr>> arguments)
{
    return std::make_shared<ConstantTarget>(selector, arguments);
}

static std::optional<mcc::FloatT> extract_float(
    const std::map<std::string, std::vector<mcc::ConstantPtr>> &arguments,
    const std::string &name)
{
    if (!arguments.contains(name))
        return std::nullopt;

    const auto values = arguments.at(name);
    if (values.empty())
        return std::nullopt;

    auto &value = values.front();

    if (const auto integer_value = std::dynamic_pointer_cast<mcc::ConstantInteger>(value))
        return integer_value->Value;

    if (const auto float_value = std::dynamic_pointer_cast<mcc::ConstantFloat>(value))
        return float_value->Value;

    throw std::runtime_error("TODO");
}

static mcc::OptionalRange<mcc::FloatT> extract_float_range(
    const std::map<std::string, std::vector<mcc::ConstantPtr>> &arguments,
    const std::string &name)
{
    if (!arguments.contains(name))
        return std::nullopt;

    const auto values = arguments.at(name);
    if (values.empty())
        return std::nullopt;

    auto &value = values.front();

    if (const auto integer_value = std::dynamic_pointer_cast<mcc::ConstantInteger>(value))
        return mcc::Range(std::optional(integer_value->Value), std::optional(integer_value->Value));

    if (const auto float_value = std::dynamic_pointer_cast<mcc::ConstantFloat>(value))
        return mcc::Range(std::optional(float_value->Value), std::optional(float_value->Value));

    if (const auto range_value = std::dynamic_pointer_cast<mcc::ConstantFloatRange>(value))
        return mcc::Range(range_value->Min, range_value->Max);

    throw std::runtime_error("TODO");
}

static std::map<std::string, mcc::Range<mcc::FloatT>> extract_scores(
    const std::map<std::string, std::vector<mcc::ConstantPtr>> &arguments,
    const std::string &name)
{
    if (!arguments.contains(name))
        return {};

    const auto values = arguments.at(name);
    if (values.empty())
        return {};

    auto &value = values.front();

    const auto object_value = std::dynamic_pointer_cast<mcc::ConstantObject>(value);
    if (!object_value)
        throw std::runtime_error("TODO");

    std::map<std::string, mcc::Range<mcc::FloatT>> scores;

    for (auto &[key_, value_]: object_value->Values)
    {
        std::optional<mcc::FloatT> min, max;

        if (const auto v_i = std::dynamic_pointer_cast<mcc::ConstantInteger>(value_))
        {
            min = v_i->Value;
            max = v_i->Value;
        }
        else if (const auto v_f = std::dynamic_pointer_cast<mcc::ConstantFloat>(value_))
        {
            min = v_f->Value;
            max = v_f->Value;
        }
        else if (const auto v_fr = std::dynamic_pointer_cast<mcc::ConstantFloatRange>(value_))
        {
            min = v_fr->Min;
            max = v_fr->Max;
        }
        else
        {
            throw std::runtime_error("TODO");
        }

        scores[key_] = {min, max};
    }

    return scores;
}

static std::vector<mcc::Invertible<std::string>> extract_tags(
    const std::map<std::string, std::vector<mcc::ConstantPtr>> &arguments,
    const std::string &name)
{
    if (!arguments.contains(name))
        return {};

    const auto values = arguments.at(name);

    std::vector<mcc::Invertible<std::string>> tags;

    for (auto &value: values)
    {
        // TODO
    }

    return tags;
}

mcc::ConstantTarget::ConstantTarget(
    const TargetSelectorE selector,
    const std::map<std::string, std::vector<ConstantPtr>> &arguments)
    : Selector(selector)
{
    X = extract_float(arguments, "x");
    Y = extract_float(arguments, "y");
    Z = extract_float(arguments, "z");
    DX = extract_float(arguments, "dx");
    DY = extract_float(arguments, "dy");
    DZ = extract_float(arguments, "dz");
    Distance = extract_float_range(arguments, "distance");
    X_Rotation = extract_float_range(arguments, "x_rotation");
    Y_Rotation = extract_float_range(arguments, "y_rotation");

    Scores = extract_scores(arguments, "scores");
    Tags = extract_tags(arguments, "tags");

    // TODO: ...
}

static mcc::Command gen_float_range(const mcc::Range<mcc::FloatT> &range)
{
    if (range.first == range.second)
        return std::to_string(range.first.value());

    const auto min_string = range.first.has_value()
                                ? std::to_string(range.first.value())
                                : "";

    const auto max_string = range.second.has_value()
                                ? std::to_string(range.second.value())
                                : "";

    return min_string + ".." + max_string;
}

mcc::CommandResult mcc::ConstantTarget::GenResult(const bool stringify) const
{
    std::string result;
    result += '@';

    switch (Selector)
    {
        case TargetSelector_P:
            result += 'p';
            break;
        case TargetSelector_R:
            result += 'r';
            break;
        case TargetSelector_A:
            result += 'a';
            break;
        case TargetSelector_E:
            result += 'e';
            break;
        case TargetSelector_S:
            result += 's';
            break;
    }

    result += '[';

    if (X.has_value())
        result += "x=" + std::to_string(X.value()) + ',';
    if (Y.has_value())
        result += "y=" + std::to_string(Y.value()) + ',';
    if (Z.has_value())
        result += "z=" + std::to_string(Z.value()) + ',';
    if (DX.has_value())
        result += "dx=" + std::to_string(DX.value()) + ',';
    if (DY.has_value())
        result += "dy=" + std::to_string(DY.value()) + ',';
    if (DZ.has_value())
        result += "dz=" + std::to_string(DZ.value()) + ',';
    if (Distance.has_value())
        result += "distance=" + gen_float_range(Distance.value()) + ',';
    if (X_Rotation.has_value())
        result += "x_rotation=" + gen_float_range(X_Rotation.value()) + ',';
    if (Y_Rotation.has_value())
        result += "y_rotation=" + gen_float_range(Y_Rotation.value()) + ',';

    // TODO: ...

    result += ']';

    if (stringify)
        result = "{selector:\"" + result + "\"}";

    return {
        .Type = CommandResultType_Value,
        .Value = result,
    };
}
