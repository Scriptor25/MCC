#include <mcc/constant.hpp>

mcc::ConstantPtr mcc::ConstantObject::Create(
    const SourceLocation &where,
    const std::map<std::string, ConstantPtr> &values)
{
    return std::make_shared<ConstantObject>(where, values);
}

mcc::ConstantObject::ConstantObject(const SourceLocation &where, const std::map<std::string, ConstantPtr> &values)
    : Constant(where, TypeID_Object),
      Values(values)
{
    for (const auto &value: Values | std::views::values)
        value->Use();
}

mcc::ConstantObject::~ConstantObject()
{
    for (const auto &value: Values | std::views::values)
        value->Drop();
}

mcc::Result mcc::ConstantObject::GenerateResult(const bool stringify) const
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
        result += key_ + ':' + value_->GenerateResult(stringify).Value;
    }

    result += '}';

    return {
        .Type = ResultType_Value,
        .Value = std::move(result),
    };
}
