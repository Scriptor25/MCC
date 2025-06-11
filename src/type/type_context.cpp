#include <mcc/type.hpp>

mcc::TypePtr mcc::TypeContext::GetVoid()
{
    if (!m_Void)
    {
        m_Void = std::make_shared<VoidType>(*this);
        m_Void->Self = m_Void;
    }
    return m_Void;
}

mcc::TypePtr mcc::TypeContext::GetNumber()
{
    if (!m_Number)
    {
        m_Number = std::make_shared<NumberType>(*this);
        m_Number->Self = m_Number;
    }
    return m_Number;
}

mcc::TypePtr mcc::TypeContext::GetString()
{
    if (!m_String)
    {
        m_String = std::make_shared<StringType>(*this);
        m_String->Self = m_String;
    }
    return m_String;
}

mcc::TypePtr mcc::TypeContext::GetArray(const TypePtr &elements)
{
    auto &type = m_Array[elements];
    if (!type)
    {
        type = std::make_shared<ArrayType>(*this, elements);
        type->Self = type;
    }
    return type;
}

mcc::TypePtr mcc::TypeContext::GetObject(const std::map<std::string, TypePtr> &elements)
{
    auto &type = m_Struct[elements];
    if (!type)
    {
        type = std::make_shared<ObjectType>(*this, elements);
        type->Self = type;
    }
    return type;
}

mcc::TypePtr mcc::TypeContext::GetTuple(const std::vector<TypePtr> &elements)
{
    auto &type = m_Tuple[elements];
    if (!type)
    {
        type = std::make_shared<TupleType>(*this, elements);
        type->Self = type;
    }
    return type;
}

mcc::TypePtr mcc::TypeContext::GetUnion(const std::set<TypePtr> &elements)
{
    auto &type = m_Union[elements];
    if (!type)
    {
        type = std::make_shared<UnionType>(*this, elements);
        type->Self = type;
    }
    return type;
}

mcc::TypePtr mcc::TypeContext::GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws)
{
    auto &type = m_Function[parameters][result][throws];
    if (!type)
    {
        type = std::make_shared<FunctionType>(*this, parameters, result, throws);
        type->Self = type;
    }
    return type;
}

mcc::TypePtr mcc::TypeContext::GetAnyArray()
{
    if (!m_AnyArray)
    {
        m_AnyArray = std::make_shared<AnyArrayType>(*this);
        m_AnyArray->Self = m_AnyArray;
    }
    return m_AnyArray;
}

mcc::TypePtr mcc::TypeContext::GetAnyObject()
{
    if (!m_AnyObject)
    {
        m_AnyObject = std::make_shared<AnyObjectType>(*this);
        m_AnyObject->Self = m_AnyObject;
    }
    return m_AnyObject;
}

mcc::TypePtr mcc::TypeContext::GetAnyFunction()
{
    if (!m_AnyFunction)
    {
        m_AnyFunction = std::make_shared<AnyFunctionType>(*this);
        m_AnyFunction->Self = m_AnyFunction;
    }
    return m_AnyFunction;
}

mcc::TypePtr mcc::TypeContext::GetUnionOrSingle(const std::set<TypePtr> &elements)
{
    return elements.size() == 1
               ? *elements.begin()
               : GetUnion(elements);
}

mcc::TypePtr mcc::TypeContext::SetNamed(const std::string &name, const TypePtr &type)
{
    auto pre = m_Named[name];
    m_Named[name] = type;
    return pre;
}

mcc::TypePtr mcc::TypeContext::GetNamed(const std::string &name) const
{
    if (m_Named.contains(name))
        return m_Named.at(name);
    return nullptr;
}
