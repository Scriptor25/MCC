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
    std::vector<ConstantPtr> values;

    for (auto &element: Elements)
    {
        auto value = element->GenerateValue(builder);

        auto constant = std::dynamic_pointer_cast<Constant>(value);
        Assert(!!constant, "inline array must only contain constant values");

        values.emplace_back(constant);
    }

    return ConstantArray::Create(values, false);
}
