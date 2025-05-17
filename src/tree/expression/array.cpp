#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::ArrayExpression::ArrayExpression(const SourceLocation &where, std::vector<ExpressionPtr> elements)
    : Expression(where),
      Elements(std::move(elements))
{
}

std::ostream &mcc::ArrayExpression::Print(std::ostream &stream) const
{
    stream << "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Elements[i]->Print(stream);
    }
    return stream << " ]";
}

mcc::ValuePtr mcc::ArrayExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;

    for (auto &element: Elements)
    {
        auto value = element->GenerateValue(builder, frame);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
        values.emplace_back(value);
    }

    if (values.size() == constants.size())
        return ConstantArray::Create(Where, constants, false);

    auto array = builder.AllocateArray(Where);

    for (const auto &value: values)
        (void) builder.CreateAppend(Where, array, value, false);

    return array;
}
