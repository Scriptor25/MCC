#include <mcc/type.hpp>
#include <utility>

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}

mcc::TypePtr mcc::TypeContext::GetVoid()
{
    if (!m_Void)
        m_Void = std::make_shared<VoidType>();
    return m_Void;
}

mcc::TypePtr mcc::TypeContext::GetNull()
{
    if (!m_Null)
        m_Null = std::make_shared<NullType>();
    return m_Null;
}

mcc::TypePtr mcc::TypeContext::GetBoolean()
{
    if (!m_Boolean)
        m_Boolean = std::make_shared<BooleanType>();
    return m_Boolean;
}

mcc::TypePtr mcc::TypeContext::GetNumber()
{
    if (!m_Number)
        m_Number = std::make_shared<NumberType>();
    return m_Number;
}

mcc::TypePtr mcc::TypeContext::GetString()
{
    if (!m_String)
        m_String = std::make_shared<StringType>();
    return m_String;
}

mcc::TypePtr mcc::TypeContext::GetArray(const TypePtr &elements)
{
    if (!m_Array.contains(elements))
        m_Array[elements] = std::make_shared<ArrayType>(elements);
    return m_Array.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetStruct(const std::map<std::string, TypePtr> &elements)
{
    if (!m_Struct.contains(elements))
        m_Struct[elements] = std::make_shared<StructType>(elements);
    return m_Struct.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetTuple(const std::vector<TypePtr> &elements)
{
    if (!m_Tuple.contains(elements))
        m_Tuple[elements] = std::make_shared<TupleType>(elements);
    return m_Tuple.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetUnion(const std::set<TypePtr> &elements)
{
    if (!m_Union.contains(elements))
        m_Union[elements] = std::make_shared<UnionType>(elements);
    return m_Union.at(elements);
}

mcc::TypePtr mcc::TypeContext::SetNamed(const std::string &name, const TypePtr &type)
{
    auto pre = m_Named[name];
    m_Named[name] = type;
    return pre;
}

mcc::TypePtr mcc::TypeContext::GetNamed(const std::string &name) const
{
    return m_Named.at(name);
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
