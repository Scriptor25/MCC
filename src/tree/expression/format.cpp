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
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
        values.emplace_back(value);

        elements.emplace(value->Type);
    }

    if (values.size() == constants.size())
        return ConstantArray::Create(Where, builder.GetContext(), constants, true);

    auto array = builder.AllocateArray(Where, builder.GetContext().GetUnion(elements));

    for (const auto &value: values)
        (void) builder.CreateAppend(Where, array, value, true);

    return array;
}
