#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ObjectType::ObjectType(TypeContext &context, const std::map<std::string, TypePtr> &elements)
    : Type(context),
      Elements(elements)
{
}

std::string mcc::ObjectType::String() const
{
    std::string result;
    result += "{ ";

    auto first = true;
    for (auto &[key, element] : Elements)
    {
        if (first)
            first = false;
        else
            result += ", ";

        result += key + ": " + element->String();
    }

    result += " }";
    return result;
}

std::ostream &mcc::ObjectType::Print(std::ostream &stream) const
{
    stream << "{ ";

    auto first = true;
    for (auto &[key, element] : Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";

        element->Print(stream << key << ": ");
    }

    return stream << " }";
}

mcc::ConstantPtr mcc::ObjectType::GetNull(const SourceLocation &where) const
{
    std::map<std::string, ConstantPtr> values;
    for (auto &[key_, element_] : Elements)
        values[key_] = element_->GetNull(where);

    return ConstantObject::Create(where, Self.lock(), values);
}

bool mcc::ObjectType::HasSpecial(const TypePtr &other) const
{
    if (other->IsAny() || !other->IsObject())
        return false;

    const auto other_object = std::dynamic_pointer_cast<ObjectType>(other);
    for (auto &[key_, element_] : Elements)
    {
        if (!other_object->Elements.contains(key_))
            return false;

        if (!SameOrSpecial(other_object->Elements[key_], element_))
            return false;
    }

    return true;
}

bool mcc::ObjectType::IsObject() const
{
    return true;
}
