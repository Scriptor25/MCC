#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::FormatExpression::FormatExpression(SourceLocation where, std::vector<FormatNodePtr> nodes)
    : Expression(std::move(where)),
      Nodes(std::move(nodes))
{
}

std::ostream &mcc::FormatExpression::Print(std::ostream &stream) const
{
    stream << '`';
    for (auto &node: Nodes)
        node->Print(stream);
    return stream << '`';
}

mcc::ValuePtr mcc::FormatExpression::Generate(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> values;

    auto all_constant = true;
    for (auto &node: Nodes)
    {
        auto value = node->Generate(builder, inline_);
        all_constant &= !!std::dynamic_pointer_cast<Constant>(value);
        values.emplace_back(value);
    }

    Assert(!inline_ || all_constant, "inline format string must only contain constant values");

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
