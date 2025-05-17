#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/format.hpp>
#include <mcc/instruction.hpp>
#include <mcc/tree.hpp>
#include <mcc/value.hpp>

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

bool mcc::FormatExpression::IsConstant() const
{
    for (auto &node: Nodes)
        if (!node->IsConstant())
            return false;
    return true;
}

bool mcc::FormatExpression::IsNull() const
{
    if (IsConstant())
        return false;
    return Expression::IsNull();
}

mcc::ValuePtr mcc::FormatExpression::GenerateValue(Builder &builder) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;

    for (auto &node: Nodes)
    {
        auto value = node->Generate(builder);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
        values.emplace_back(value);
    }

    if (values.size() == constants.size())
        return ConstantArray::Create(constants, true);

    auto array = builder.AllocateArray();

    for (const auto &value: values)
        (void) builder.CreateAppend(array, value, true);

    return array;
}
