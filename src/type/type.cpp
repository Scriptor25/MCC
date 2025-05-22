#include <mcc/type.hpp>
#include <utility>

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}

std::ostream &mcc::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

std::ostream &mcc::NullType::Print(std::ostream &stream) const
{
    return stream << "null";
}

std::ostream &mcc::BooleanType::Print(std::ostream &stream) const
{
    return stream << "boolean";
}

std::ostream &mcc::NumberType::Print(std::ostream &stream) const
{
    return stream << "number";
}

std::ostream &mcc::StringType::Print(std::ostream &stream) const
{
    return stream << "string";
}

mcc::ArrayType::ArrayType(TypePtr elements)
    : Elements(std::move(elements))
{
}

std::ostream &mcc::ArrayType::Print(std::ostream &stream) const
{
    return Elements->Print(stream) << "[]";
}

mcc::StructType::StructType(const std::map<std::string, TypePtr> &elements)
    : Elements(elements)
{
}

std::ostream &mcc::StructType::Print(std::ostream &stream) const
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

mcc::TupleType::TupleType(const std::vector<TypePtr> &elements)
    : Elements(elements)
{
}

std::ostream &mcc::TupleType::Print(std::ostream &stream) const
{
    stream << "[ ";

    auto first = true;
    for (auto &element: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        element->Print(stream);
    }

    return stream << " ]";
}

mcc::UnionType::UnionType(const std::set<TypePtr> &elements)
    : Elements(elements)
{
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
