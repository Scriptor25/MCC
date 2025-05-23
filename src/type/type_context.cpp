#include <mcc/type.hpp>

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
    auto &type = m_Array[elements];
    if (!type)
        type = std::make_shared<ArrayType>(elements);
    return type;
}

mcc::TypePtr mcc::TypeContext::GetStruct(const std::map<std::string, TypePtr> &elements)
{
    auto &type = m_Struct[elements];
    if (!type)
        type = std::make_shared<StructType>(elements);
    return type;
}

mcc::TypePtr mcc::TypeContext::GetTuple(const std::vector<TypePtr> &elements)
{
    auto &type = m_Tuple[elements];
    if (!type)
        type = std::make_shared<TupleType>(elements);
    return type;
}

mcc::TypePtr mcc::TypeContext::GetUnion(const std::set<TypePtr> &elements)
{
    auto &type = m_Union[elements];
    if (!type)
        type = std::make_shared<UnionType>(elements);
    return type;
}

mcc::TypePtr mcc::TypeContext::GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws)
{
    auto &type = m_Function[parameters][result][throws];
    if (!type)
        type = std::make_shared<FunctionType>(parameters, result, throws);
    return type;
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
