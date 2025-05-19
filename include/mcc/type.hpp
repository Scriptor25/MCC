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
        static TypePtr GetArray(const TypePtr &elements);
        static TypePtr GetStruct(const std::map<std::string, TypePtr> &elements);
        static TypePtr GetTuple(const std::vector<TypePtr> &elements);
        static TypePtr GetUnion(const std::set<TypePtr> &elements);

    private:
        std::shared_ptr<Type> m_Void, m_Null, m_Boolean, m_Number, m_String;
        std::map<TypePtr, std::shared_ptr<Type>> m_Array;
        std::map<std::map<std::string, TypePtr>, std::shared_ptr<Type>> m_Struct;
        std::map<std::vector<TypePtr>, std::shared_ptr<Type>> m_Tuple;
        std::map<std::set<TypePtr>, std::shared_ptr<Type>> m_Union;
    };

    struct Type
    {
        virtual ~Type() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };

    struct VoidType final : Type
    {
        VoidType() = default;

        std::ostream &Print(std::ostream &stream) const override;
    };

    struct NullType final : Type
    {
        NullType() = default;

        std::ostream &Print(std::ostream &stream) const override;
    };

    struct BooleanType final : Type
    {
        BooleanType() = default;

        std::ostream &Print(std::ostream &stream) const override;
    };

    struct NumberType final : Type
    {
        NumberType() = default;

        std::ostream &Print(std::ostream &stream) const override;
    };

    struct StringType final : Type
    {
        StringType() = default;

        std::ostream &Print(std::ostream &stream) const override;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        explicit ArrayType(const TypePtr &elements);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Elements;
    };

    /** { a: A, ..., b: B } */
    struct StructType final : Type
    {
        explicit StructType(const std::map<std::string, TypePtr> &elements);

        std::ostream &Print(std::ostream &stream) const override;

        std::map<std::string, TypePtr> Elements;
    };

    /** [ A, ..., B ] */
    struct TupleType final : Type
    {
        explicit TupleType(const std::vector<TypePtr> &elements);

        std::ostream &Print(std::ostream &stream) const override;

        std::vector<TypePtr> Elements;
    };

    /** A | ... | B */
    struct UnionType final : Type
    {
        explicit UnionType(const std::set<TypePtr> &elements);

        std::ostream &Print(std::ostream &stream) const override;

        std::set<TypePtr> Elements;
    };
}
