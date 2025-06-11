#include <mcc/error.hpp>
#include <mcc/type.hpp>

mcc::UnionType::UnionType(TypeContext &context, const std::set<TypePtr> &elements)
    : Type(context),
      Elements(elements)
{
    Assert(elements.size() > 1, "union can only be constructed from two or more types");
}

std::string mcc::UnionType::String() const
{
    std::string result;
    result += '(';

    auto first = true;
    for (auto &element : Elements)
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
    for (auto &element : Elements)
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
    Error(where, "cannot get null value for type {}", Self.lock());
}

bool mcc::UnionType::HasSpecial(const TypePtr &other) const
{
    for (auto &element : Elements)
        if (SameOrSpecial(other, element))
            return true;

    if (other->IsAny() || !other->IsUnion())
        return false;

    for (const auto other_union = std::dynamic_pointer_cast<UnionType>(other);
         auto &other_element : other_union->Elements)
        if (!SameOrSpecial(other_element, Self.lock()))
            return false;

    return true;
}

bool mcc::UnionType::IsUnion() const
{
    return true;
}
