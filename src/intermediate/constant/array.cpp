#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(std::vector<ConstantPtr> values, bool stringify)
{
    return std::make_shared<ConstantArray>(std::move(values), stringify);
}

mcc::ConstantArray::ConstantArray(std::vector<ConstantPtr> values, const bool stringify)
    : Values(std::move(values)),
      Stringify(stringify)
{
    for (const auto &value: Values)
        value->Use();
}

mcc::ConstantArray::~ConstantArray()
{
    for (const auto &value: Values)
        value->Drop();
}

mcc::Result mcc::ConstantArray::GenResult(const bool stringify, const bool use_stack) const
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
        result += value->GenResult(Stringify || stringify, use_stack).Value;
    }

    result += ']';
    return {
        .Type = ResultType_Value,
        .Value = result,
    };
}
