#include <mcc/type.hpp>

mcc::TypeContext mcc::TypeContext::GlobalContext = TypeContext();

mcc::TypePtr mcc::TypeContext::GetVoid()
{
    if (!GlobalContext.m_Void)
        GlobalContext.m_Void = std::make_unique<VoidType>();
    return GlobalContext.m_Void.get();
}

mcc::TypePtr mcc::TypeContext::GetNull()
{
    if (!GlobalContext.m_Null)
        GlobalContext.m_Null = std::make_unique<NullType>();
    return GlobalContext.m_Null.get();
}

mcc::TypePtr mcc::TypeContext::GetBoolean()
{
    if (!GlobalContext.m_Boolean)
        GlobalContext.m_Boolean = std::make_unique<BooleanType>();
    return GlobalContext.m_Boolean.get();
}

mcc::TypePtr mcc::TypeContext::GetNumber()
{
    if (!GlobalContext.m_Number)
        GlobalContext.m_Number = std::make_unique<NumberType>();
    return GlobalContext.m_Number.get();
}

mcc::TypePtr mcc::TypeContext::GetString()
{
    if (!GlobalContext.m_String)
        GlobalContext.m_String = std::make_unique<StringType>();
    return GlobalContext.m_String.get();
}

mcc::TypePtr mcc::TypeContext::GetArray(TypePtr elements)
{
    if (!GlobalContext.m_Array.contains(elements))
        GlobalContext.m_Array[elements] = std::make_unique<ArrayType>(elements);
    return GlobalContext.m_Array.at(elements).get();
}

mcc::TypePtr mcc::TypeContext::GetStruct(const std::map<std::string, TypePtr> &elements)
{
    if (!GlobalContext.m_Struct.contains(elements))
        GlobalContext.m_Struct[elements] = std::make_unique<StructType>(elements);
    return GlobalContext.m_Struct.at(elements).get();
}

mcc::TypePtr mcc::TypeContext::GetTuple(const std::vector<TypePtr> &elements)
{
    if (!GlobalContext.m_Tuple.contains(elements))
        GlobalContext.m_Tuple[elements] = std::make_unique<TupleType>(elements);
    return GlobalContext.m_Tuple.at(elements).get();
}

mcc::TypePtr mcc::TypeContext::GetUnion(const std::set<TypePtr> &elements)
{
    if (!GlobalContext.m_Union.contains(elements))
        GlobalContext.m_Union[elements] = std::make_unique<UnionType>(elements);
    return GlobalContext.m_Union.at(elements).get();
}

mcc::ArrayType::ArrayType(TypePtr elements)
    : Elements(elements)
{
}

mcc::StructType::StructType(const std::map<std::string, TypePtr> &elements)
    : Elements(elements)
{
}

mcc::TupleType::TupleType(const std::vector<TypePtr> &elements)
    : Elements(elements)
{
}

mcc::UnionType::UnionType(const std::set<TypePtr> &elements)
    : Elements(elements)
{
}
