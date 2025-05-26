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
    for (auto &[key, element]: Elements)
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
    for (auto &[key, element]: Elements)
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
    for (auto &[key_, element_]: Elements)
    {
        const auto value = element_->GetNull(where);
        if (!value)
        {
            return nullptr;
        }
        values[key_] = value;
    }
    return ConstantObject::Create(where, Context, values);
}

bool mcc::ObjectType::IsObject() const
{
    return true;
}
