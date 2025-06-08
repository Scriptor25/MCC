#include <set>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/format.hpp>
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
    for (auto &node : Nodes)
        node->Print(stream);
    return stream << '`';
}

mcc::ValuePtr mcc::FormatExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;

    std::set<TypePtr> elements;

    for (auto &node : Nodes)
    {
        auto value = node->Generate(builder, frame);
        values.emplace_back(value);
        elements.insert(value->Type);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
    }

    if (values.size() == constants.size())
        return ConstantArray::Create(Where, builder.GetContext(), constants, true);

    const auto type = builder.GetContext().GetArray(builder.GetContext().GetUnionOrSingle(elements));

    auto array = builder.Allocate(Where, type, false);
    (void) builder.CreateStore(Where, array, type->GetNull(Where), true);

    for (const auto &value : values)
        (void) builder.CreateAppend(Where, array, StringifyValue::Create(Where, value), true);

    return array;
}
