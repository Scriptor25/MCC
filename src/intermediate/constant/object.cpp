#include <mcc/constant.hpp>
#include <mcc/type.hpp>

#include <ranges>

mcc::ConstantPtr mcc::ConstantObject::Create(
        const SourceLocation &where,
        const TypePtr &type,
        const std::map<
                std::string,
                ConstantPtr
        > &values)
{
    auto self = std::make_shared<ConstantObject>(where, type, values);

    self->Self = self;
    for (const auto &value : self->Values | std::views::values)
        value->Use(self);

    return self;
}

mcc::ConstantPtr mcc::ConstantObject::Create(
        const SourceLocation &where,
        TypeContext &context,
        const std::map<
                std::string,
                ConstantPtr
        > &values)
{
    std::map<std::string, TypePtr> elements;
    for (const auto &[name_, value_] : values)
        elements[name_] = value_->Type;

    return Create(where, context.GetObject(elements), values);
}

mcc::ConstantObject::ConstantObject(
        const SourceLocation &where,
        const TypePtr &type,
        const std::map<
                std::string,
                ConstantPtr
        > &values)
    : Constant(
              where,
              type),
      Values(values)
{
}

mcc::ConstantObject::~ConstantObject()
{
    for (const auto &value : Values | std::views::values)
        value->Drop(Self);
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
