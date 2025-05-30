#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::TupleType::TupleType(TypeContext &context, const std::vector<TypePtr> &elements)
    : Type(context),
      Elements(elements)
{
}

std::string mcc::TupleType::String() const
{
    std::string result;
    result += "[ ";

    auto first = true;
    for (auto &element : Elements)
    {
        if (first)
            first = false;
        else
            result += ", ";
        result += element->String();
    }

    result += " ]";
    return result;
}

std::ostream &mcc::TupleType::Print(std::ostream &stream) const
{
    stream << "[ ";

    auto first = true;
    for (auto &element : Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        element->Print(stream);
    }

    return stream << " ]";
}

mcc::ConstantPtr mcc::TupleType::GetNull(const SourceLocation &where) const
{
    std::vector<ConstantPtr> values;

    for (auto &element : Elements)
    {
        auto value = element->GetNull(where);
        if (!value)
            return nullptr;

        values.emplace_back(value);
    }

    return ConstantArray::Create(where, Self.lock(), values, false);
}

bool mcc::TupleType::IsTuple() const
{
    return true;
}
