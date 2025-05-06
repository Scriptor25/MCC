#include <mcc/attribute.hpp>
#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantTarget::Create(
    TargetSelectorE selector,
    std::map<std::string, std::vector<TargetAttributePtr>> attributes)
{
    return std::make_shared<ConstantTarget>(selector, std::move(attributes));
}

mcc::ConstantTarget::ConstantTarget(
    const TargetSelectorE selector,
    std::map<std::string, std::vector<TargetAttributePtr>> attributes)
    : Selector(selector),
      Attributes(std::move(attributes))
{
}

mcc::Result mcc::ConstantTarget::GenerateResult(const bool stringify, bool use_stack) const
{
    std::string result;
    result += '@';
    result += ToString(Selector);

    if (!Attributes.empty())
    {
        result += '[';

        auto first = true;
        for (auto &[key_, value_]: Attributes)
            for (auto &attribute: value_)
            {
                if (first)
                    first = false;
                else
                    result += ',';

                result += key_ + '=' + attribute->String();
            }

        result += ']';
    }

    if (stringify)
        result = "{selector:\"" + result + "\"}";

    return {
        .Type = ResultType_Value,
        .Value = std::move(result),
    };
}
