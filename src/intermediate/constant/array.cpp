#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(
        const SourceLocation &where,
        const TypePtr &type,
        const std::vector<ConstantPtr> &values,
        const bool stringify)
{
    auto self = std::make_shared<ConstantArray>(where, type, values, stringify);

    self->Self = self;
    for (const auto &value : self->Values)
        value->Use(self);

    return self;
}

mcc::ConstantPtr mcc::ConstantArray::Create(
        const SourceLocation &where,
        Context &context,
        const std::vector<ConstantPtr> &values,
        const bool stringify)
{
    std::set<TypePtr> elements;
    for (auto &value : values)
        elements.insert(value->Type);

    auto type = context.GetArray(context.GetUnionOrSingle(elements));

    return Create(where, type, values, stringify);
}

mcc::ConstantArray::ConstantArray(
        const SourceLocation &where,
        const TypePtr &type,
        const std::vector<ConstantPtr> &values,
        const bool stringify)
    : Constant(
              where,
              type),
      Values(values),
      Stringify(stringify)
{
}

mcc::ConstantArray::~ConstantArray()
{
    for (const auto &value : Values)
        value->Drop(Self);
}

mcc::Result mcc::ConstantArray::GenerateResult() const
{
    std::string result;
    result += '[';

    auto first = true;
    for (auto &value : Values)
    {
        if (first)
            first = false;
        else
            result += ',';

        result += value->GenerateResult().Value;
    }

    result += ']';

    return {
        .Type    = ResultType_::Value,
        .Value   = std::move(result),
        .NotNull = true,
    };
}
