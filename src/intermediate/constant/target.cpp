#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

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

    auto first = true;
    for (auto &[key_, value_]: Attributes)
    {
        for (auto &attribute: value_)
        {
            if (first)
                first = false;
            else
                result += ',';
            result += key_;
            result += '=';
            result += attribute->String();
        }
    }

    result += ']';

    if (stringify)
        result = "{selector:\"" + result + "\"}";

    return {
        .Type = CommandResultType_Value,
        .Value = result,
    };
}
