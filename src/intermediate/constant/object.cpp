#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantObject::Create(std::map<std::string, ConstantPtr> values)
{
    return std::make_shared<ConstantObject>(std::move(values));
}

mcc::ConstantObject::ConstantObject(std::map<std::string, ConstantPtr> values)
    : Values(std::move(values))
{
    for (const auto &value: Values | std::views::values)
        value->Use();
}

mcc::ConstantObject::~ConstantObject()
{
    for (const auto &value: Values | std::views::values)
        value->Drop();
}

mcc::Result mcc::ConstantObject::GenResult(const bool stringify, const bool use_stack) const
{
    std::string result;
    result += '{';

    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += key_ + ':' + value_->GenResult(stringify, use_stack).Value;
    }

    result += '}';

    return {
        .Type = ResultType_Value,
        .Value = result,
    };
}
