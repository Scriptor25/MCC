#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(std::vector<ConstantPtr> values, bool stringify)
{
    return std::make_shared<ConstantArray>(std::move(values), stringify);
}

mcc::ConstantArray::ConstantArray(std::vector<ConstantPtr> values, const bool stringify)
    : Values(std::move(values)),
      Stringify(stringify)
{
}

mcc::CommandResult mcc::ConstantArray::GenResult(const bool stringify) const
{
    std::string result;
    result += '[';

    auto first = true;
    for (auto &value: Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += value->GenResult(Stringify || stringify).Value;
    }

    result += ']';
    return {
        .Type = CommandResultType_Value,
        .Value = result,
    };
}
