#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::FormatExpression::FormatExpression(std::vector<FormatNodePtr> nodes)
    : Nodes(std::move(nodes))
{
}

std::ostream &mcc::FormatExpression::Print(std::ostream &stream) const
{
    stream << '`';
    for (auto &node: Nodes)
        node->Print(stream);
    return stream << '`';
}

mcc::ValuePtr mcc::FormatExpression::Gen(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> values;

    auto all_constant = true;
    for (auto &node: Nodes)
    {
        auto value = node->Gen(builder, inline_);
        all_constant &= !!std::dynamic_pointer_cast<Constant>(value);
        values.emplace_back(value);
    }

    if (inline_ && !all_constant)
        throw std::runtime_error("TODO");

    if (all_constant)
    {
        std::vector<ConstantPtr> constants;

        for (auto &value: values)
            constants.emplace_back(std::dynamic_pointer_cast<Constant>(value));

        return ConstantArray::Create(constants, true);
    }

    auto array = builder.AllocateArray(inline_);

    for (const auto &value: values)
        builder.CreateAppend(array, value, true, inline_);

    return array;
}
