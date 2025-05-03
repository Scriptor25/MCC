#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(std::vector<ConstantPtr> values)
{
    return std::make_shared<ConstantArray>(std::move(values));
}

mcc::ConstantArray::ConstantArray(std::vector<ConstantPtr> values)
    : Values(std::move(values))
{
}

mcc::Command mcc::ConstantArray::GenInline() const
{
    Command result;
    result += '[';

    auto first = true;
    for (auto &value: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += value->GenInline();
    }

    result += ']';
    return result;
}
