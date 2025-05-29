#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantObject::Create(const SourceLocation &where, const TypePtr &type, const std::map<std::string, ConstantPtr> &values)
{
    return std::make_shared<ConstantObject>(where, type, values);
}

mcc::ConstantPtr mcc::ConstantObject::Create(const SourceLocation &where, TypeContext &context, const std::map<std::string, ConstantPtr> &values)
{
    std::map<std::string, TypePtr> elements;
    for (const auto &[name_, value_] : values)
        elements[name_] = value_->Type;

    return std::make_shared<ConstantObject>(where, context.GetObject(elements), values);
}

mcc::ConstantObject::ConstantObject(const SourceLocation &where, const TypePtr &type, const std::map<std::string, ConstantPtr> &values)
    : Constant(where, type), Values(values)
{
    for (const auto &value : Values | std::views::values)
        value->Use();
}

mcc::ConstantObject::~ConstantObject()
{
    for (const auto &value : Values | std::views::values)
        value->Drop();
}

mcc::Result mcc::ConstantObject::GenerateResult() const
{
    std::string result;
    result += '{';

    auto first = true;
    for (auto &[key_, value_] : Values)
    {
        if (first)
            first = false;
        else
            result += ',';
        result += '"' + key_ + "\":" + value_->GenerateResult().Value;
    }

    result += '}';

    return {
        .Type    = ResultType_Value,
        .Value   = std::move(result),
        .NotNull = true,
    };
}
