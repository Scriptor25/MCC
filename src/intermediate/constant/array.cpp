#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantArray::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const std::vector<ConstantPtr> &values,
    const bool stringify)
{
    return std::make_shared<ConstantArray>(where, type, values, stringify);
}

mcc::ConstantPtr mcc::ConstantArray::Create(
    const SourceLocation &where,
    TypeContext &context,
    const std::vector<ConstantPtr> &values,
    const bool stringify)
{
    std::set<TypePtr> elements;
    for (auto &value : values)
        elements.insert(value->Type);

    auto type = context.GetArray(
        elements.size() == 1
            ? *elements.begin()
            : context.GetUnion(elements));

    return std::make_shared<ConstantArray>(where, type, values, stringify);
}

mcc::ConstantArray::ConstantArray(
    const SourceLocation &where,
    const TypePtr &type,
    const std::vector<ConstantPtr> &values,
    const bool stringify)
    : Constant(where, type),
      Values(values),
      Stringify(stringify)
{
    for (const auto &value : Values)
        value->Use();
}

mcc::ConstantArray::~ConstantArray()
{
    for (const auto &value : Values)
        value->Drop();
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
        .Type = ResultType_Value,
        .Value = std::move(result),
        .NotNull = true,
    };
}
