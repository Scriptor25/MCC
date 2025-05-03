#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantObject::Create(std::map<std::string, ConstantPtr> values)
{
    return std::make_shared<ConstantObject>(std::move(values));
}

mcc::ConstantObject::ConstantObject(std::map<std::string, ConstantPtr> values)
    : Values(std::move(values))
{
}

mcc::CommandResult mcc::ConstantObject::GenResult(const bool stringify) const
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
        result += key_ + ':' + value_->GenResult(stringify).Value;
    }

    result += '}';

    return {
        .Type = CommandResultType_Value,
        .Value = result,
    };
}
