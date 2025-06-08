#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ArrayExpression::ArrayExpression(const SourceLocation &where, std::vector<ExpressionPtr> elements, TypePtr type)
    : Expression(where),
      Elements(std::move(elements)),
      Type(std::move(type))
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
    stream << " ]";

    if (Type)
        Type->Print(stream << ':');

    return stream;
}

mcc::ValuePtr mcc::ArrayExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> values;
    std::vector<ConstantPtr> constants;
    std::set<TypePtr> elements;

    for (auto &element : Elements)
    {
        auto value = element->GenerateValue(builder, frame);
        values.emplace_back(value);
        elements.insert(value->Type);
        if (auto constant = std::dynamic_pointer_cast<Constant>(value))
            constants.emplace_back(constant);
    }

    const auto type = Type ? Type : builder.GetContext().GetArray(builder.GetContext().GetUnionOrSingle(elements));

    if (values.size() == constants.size())
        return ConstantArray::Create(Where, type, constants, false);

    auto array = builder.Allocate(Where, type, false);
    (void) builder.CreateStore(Where, array, type->GetNull(Where), true);

    for (const auto &value : values)
        (void) builder.CreateAppend(Where, array, value, true);

    return array;
}
