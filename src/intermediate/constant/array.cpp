#include <mcc/constant.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(
    const SourceLocation &where,
    const std::vector<ConstantPtr> &values,
    const bool stringify)
{
    return std::make_shared<ConstantArray>(where, values, stringify);
}

mcc::ConstantArray::ConstantArray(
    const SourceLocation &where,
    const std::vector<ConstantPtr> &values,
    const bool stringify)
    : Constant(where, TypeID_Array),
      Values(values),
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

mcc::Result mcc::ConstantArray::GenerateResult(const bool stringify) const
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

        result += value->GenerateResult(Stringify || stringify).Value;
    }

    result += ']';

    return {
        .Type = ResultType_Value,
        .Value = std::move(result),
    };
}
