#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantObject::Create(std::map<std::string, ConstantPtr> values)
{
    return std::make_shared<ConstantObject>(std::move(values));
}

mcc::ConstantObject::ConstantObject(std::map<std::string, ConstantPtr> values)
    : Values(std::move(values))
{
}

mcc::Command mcc::ConstantObject::GenInline() const
{
    Command result;
    result += '{';

    auto first = true;
    for (auto &[key_, value_]: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += key_;
        result += ':';
        result += value_->GenInline();
    }

    result += '}';
    return result;
}
