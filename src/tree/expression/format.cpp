#include <set>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/format.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::FormatExpression::FormatExpression(const SourceLocation &where, std::vector<FormatNodePtr> nodes)
    : Expression(where),
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

mcc::ValuePtr mcc::FormatExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;

    std::set<TypePtr> elements;

    for (auto &node: Nodes)
    {
        auto value = node->Generate(builder, frame);
        values.emplace_back(value);

        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
        {
            constants.emplace_back(constant);
        }

        elements.insert(value->Type);
    }

    if (values.size() == constants.size())
    {
        return ConstantArray::Create(Where, builder.GetContext(), constants, true);
    }

    const auto type = builder.GetContext().GetArray(
        elements.size() == 1
            ? *elements.begin()
            : builder.GetContext().GetUnion(elements));

    auto array = builder.CreateAllocation(Where, type, false, nullptr);

    for (auto value: values)
    {
        (void) builder.CreateAppend(Where, array, StringifyValue::Create(Where, value));
    }

    return array;
}
