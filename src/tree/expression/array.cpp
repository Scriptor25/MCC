#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ArrayExpression::ArrayExpression(SourceLocation where, std::vector<ExpressionPtr> elements)
    : Expression(std::move(where)),
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

mcc::ValuePtr mcc::ArrayExpression::GenerateValue(Builder &builder) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;

    for (auto &element: Elements)
    {
        auto value = element->GenerateValue(builder);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
        values.emplace_back(value);
    }

    if (values.size() == constants.size())
        return ConstantArray::Create(std::move(constants), false);

    auto array = builder.AllocateArray();

    for (const auto &value: values)
        (void) builder.CreateAppend(array, value, false);

    return array;
}
