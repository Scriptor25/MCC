#include <mcc/type.hpp>

std::ostream &mcc::operator<<(std::ostream &stream, const TypePtr &type)
{
    return type->Print(stream);
}

mcc::TypeContext mcc::TypeContext::GlobalContext = TypeContext();

mcc::TypePtr mcc::TypeContext::GetVoid()
{
    if (!GlobalContext.m_Void)
        GlobalContext.m_Void = std::make_shared<VoidType>();
    return GlobalContext.m_Void;
}

mcc::TypePtr mcc::TypeContext::GetNull()
{
    if (!GlobalContext.m_Null)
        GlobalContext.m_Null = std::make_shared<NullType>();
    return GlobalContext.m_Null;
}

mcc::TypePtr mcc::TypeContext::GetBoolean()
{
    if (!GlobalContext.m_Boolean)
        GlobalContext.m_Boolean = std::make_shared<BooleanType>();
    return GlobalContext.m_Boolean;
}

mcc::TypePtr mcc::TypeContext::GetNumber()
{
    if (!GlobalContext.m_Number)
        GlobalContext.m_Number = std::make_shared<NumberType>();
    return GlobalContext.m_Number;
}

mcc::TypePtr mcc::TypeContext::GetString()
{
    if (!GlobalContext.m_String)
        GlobalContext.m_String = std::make_shared<StringType>();
    return GlobalContext.m_String;
}

mcc::TypePtr mcc::TypeContext::GetArray(const TypePtr &elements)
{
    if (!GlobalContext.m_Array.contains(elements))
        GlobalContext.m_Array[elements] = std::make_shared<ArrayType>(elements);
    return GlobalContext.m_Array.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetStruct(const std::map<std::string, TypePtr> &elements)
{
    if (!GlobalContext.m_Struct.contains(elements))
        GlobalContext.m_Struct[elements] = std::make_shared<StructType>(elements);
    return GlobalContext.m_Struct.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetTuple(const std::vector<TypePtr> &elements)
{
    if (!GlobalContext.m_Tuple.contains(elements))
        GlobalContext.m_Tuple[elements] = std::make_shared<TupleType>(elements);
    return GlobalContext.m_Tuple.at(elements);
}

mcc::TypePtr mcc::TypeContext::GetUnion(const std::set<TypePtr> &elements)
{
    if (!GlobalContext.m_Union.contains(elements))
        GlobalContext.m_Union[elements] = std::make_shared<UnionType>(elements);
    return GlobalContext.m_Union.at(elements);
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

mcc::ArrayType::ArrayType(const TypePtr &elements)
    : Elements(elements)
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
