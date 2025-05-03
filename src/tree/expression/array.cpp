#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ArrayExpression::ArrayExpression(std::vector<ExpressionPtr> elements)
    : Elements(std::move(elements))
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

mcc::ValuePtr mcc::ArrayExpression::Gen(Builder &builder, const bool inline_) const
{
    std::vector<ConstantPtr> values;

    for (auto &element: Elements)
    {
        auto value = element->Gen(builder, inline_);

        auto constant = std::dynamic_pointer_cast<Constant>(value);
        if (!constant)
            throw std::runtime_error("non-constant value in constant array");

        values.emplace_back(constant);
    }

    return ConstantArray::Create(values, false);
}
