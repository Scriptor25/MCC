#pragma once

#include <map>
#include <set>
#include <vector>
#include <mcc/common.hpp>

namespace mcc
{
    class TypeContext
    {
    public:
        static TypeContext GlobalContext;

        static TypePtr GetVoid();
        static TypePtr GetNull();
        static TypePtr GetBoolean();
        static TypePtr GetNumber();
        static TypePtr GetString();
        static TypePtr GetArray(TypePtr elements);
        static TypePtr GetStruct(const std::map<std::string, TypePtr> &elements);
        static TypePtr GetTuple(const std::vector<TypePtr> &elements);
        static TypePtr GetUnion(const std::set<TypePtr> &elements);

    private:
        std::unique_ptr<Type> m_Void, m_Null, m_Boolean, m_Number, m_String;
        std::map<TypePtr, std::unique_ptr<Type>> m_Array;
        std::map<std::map<std::string, TypePtr>, std::unique_ptr<Type>> m_Struct;
        std::map<std::vector<TypePtr>, std::unique_ptr<Type>> m_Tuple;
        std::map<std::set<TypePtr>, std::unique_ptr<Type>> m_Union;
    };

    struct Type
    {
        virtual ~Type() = default;
    };

    struct VoidType final : Type
    {
        VoidType() = default;
    };

    struct NullType final : Type
    {
        NullType() = default;
    };

    struct BooleanType final : Type
    {
        BooleanType() = default;
    };

    struct NumberType final : Type
    {
        NumberType() = default;
    };

    struct StringType final : Type
    {
        StringType() = default;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        explicit ArrayType(TypePtr elements);

        TypePtr Elements;
    };

    /** { a: A, ..., b: B } */
    struct StructType final : Type
    {
        explicit StructType(const std::map<std::string, TypePtr> &elements);

        std::map<std::string, TypePtr> Elements;
    };

    /** [ A, ..., B ] */
    struct TupleType final : Type
    {
        explicit TupleType(const std::vector<TypePtr> &elements);

        std::vector<TypePtr> Elements;
    };

    /** A | ... | B */
    struct UnionType final : Type
    {
        explicit UnionType(const std::set<TypePtr> &elements);

        std::set<TypePtr> Elements;
    };
}
