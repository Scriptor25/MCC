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
        TypePtr GetVoid();
        TypePtr GetNull();
        TypePtr GetBoolean();
        TypePtr GetNumber();
        TypePtr GetString();
        TypePtr GetArray(const TypePtr &elements);
        TypePtr GetStruct(const std::map<std::string, TypePtr> &elements);
        TypePtr GetTuple(const std::vector<TypePtr> &elements);
        TypePtr GetUnion(const std::set<TypePtr> &elements);
        TypePtr GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        TypePtr SetNamed(const std::string &name, const TypePtr &type);
        TypePtr GetNamed(const std::string &name) const;

    private:
        std::map<std::string, TypePtr> m_Named;

        TypePtr m_Void, m_Null, m_Boolean, m_Number, m_String;
        std::map<TypePtr, std::shared_ptr<struct ArrayType>> m_Array;
        std::map<std::map<std::string, TypePtr>, std::shared_ptr<struct StructType>> m_Struct;
        std::map<std::vector<TypePtr>, std::shared_ptr<struct TupleType>> m_Tuple;
        std::map<std::set<TypePtr>, std::shared_ptr<struct UnionType>> m_Union;
        std::map<std::vector<TypePtr>, std::map<TypePtr, std::map<bool, std::shared_ptr<struct FunctionType>>>>
        m_Function;
    };

    struct Type
    {
        virtual ~Type() = default;

        virtual std::string String() const = 0;
        virtual std::ostream &Print(std::ostream &stream) const = 0;
    };

    struct VoidType final : Type
    {
        VoidType() = default;

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    struct NullType final : Type
    {
        NullType() = default;

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    struct BooleanType final : Type
    {
        BooleanType() = default;

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    struct NumberType final : Type
    {
        NumberType() = default;

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    struct StringType final : Type
    {
        StringType() = default;

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        explicit ArrayType(TypePtr elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Elements;
    };

    /** { a: A, ..., b: B } */
    struct StructType final : Type
    {
        explicit StructType(const std::map<std::string, TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        std::map<std::string, TypePtr> Elements;
    };

    /** [ A, ..., B ] */
    struct TupleType final : Type
    {
        explicit TupleType(const std::vector<TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        std::vector<TypePtr> Elements;
    };

    /** A | ... | B */
    struct UnionType final : Type
    {
        explicit UnionType(const std::set<TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        std::set<TypePtr> Elements;
    };

    /** $(A, ..., B) => C, $!(A, ..., B) => C */
    struct FunctionType final : Type
    {
        FunctionType(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        std::vector<TypePtr> Parameters;
        TypePtr Result;
        bool Throws;
    };
}

namespace std
{
    template<>
    struct formatter<mcc::TypePtr> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::TypePtr &type, FormatContext &ctx) const
        {
            return formatter<string>::format(type->String(), ctx);
        }
    };
}
