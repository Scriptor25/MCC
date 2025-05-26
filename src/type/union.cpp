#include <mcc/type.hpp>

mcc::UnionType::UnionType(TypeContext &context, const std::set<TypePtr> &elements)
    : Type(context),
      Elements(elements)
{
}

std::string mcc::UnionType::String() const
{
    std::string result;
    result += '(';

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            result += " | ";
        result += element->String();
    }

    result += ')';
    return result;
}

std::ostream &mcc::UnionType::Print(std::ostream &stream) const
{
    stream << '(';

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            stream << " | ";
        element->Print(stream);
    }

    return stream << ')';
}

mcc::ConstantPtr mcc::UnionType::GetNull(const SourceLocation &where) const
{
    return nullptr;
}

bool mcc::UnionType::IsUnion() const
{
    return true;
}
