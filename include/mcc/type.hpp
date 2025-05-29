#pragma once

#include <map>
#include <mcc/common.hpp>
#include <set>
#include <vector>

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
        TypePtr GetObject(const std::map<std::string, TypePtr> &elements);
        TypePtr GetTuple(const std::vector<TypePtr> &elements);
        TypePtr GetUnion(const std::set<TypePtr> &elements);
        TypePtr GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        TypePtr SetNamed(const std::string &name, const TypePtr &type);
        TypePtr GetNamed(const std::string &name) const;

    private:
        std::map<std::string, TypePtr> m_Named;

        TypePtr m_Void, m_Null, m_Boolean, m_Number, m_String;
        std::map<TypePtr, std::shared_ptr<struct ArrayType>> m_Array;
        std::map<std::map<std::string, TypePtr>, std::shared_ptr<struct ObjectType>> m_Struct;
        std::map<std::vector<TypePtr>, std::shared_ptr<struct TupleType>> m_Tuple;
        std::map<std::set<TypePtr>, std::shared_ptr<struct UnionType>> m_Union;
        std::map<std::vector<TypePtr>, std::map<TypePtr, std::map<bool, std::shared_ptr<struct FunctionType>>>> m_Function;
    };

    struct Type
    {
        explicit Type(TypeContext &context);
        virtual ~Type() = default;

        virtual std::string String() const                      = 0;
        virtual std::ostream &Print(std::ostream &stream) const = 0;

        virtual ConstantPtr GetNull(const SourceLocation &where) const = 0;

        virtual bool IsVoid() const;
        virtual bool IsNull() const;
        virtual bool IsBoolean() const;
        virtual bool IsNumber() const;
        virtual bool IsString() const;
        virtual bool IsArray() const;
        virtual bool IsObject() const;
        virtual bool IsTuple() const;
        virtual bool IsUnion() const;
        virtual bool IsFunction() const;

        TypeContext &Context;
        std::weak_ptr<Type> Self;
    };

    struct VoidType final : Type
    {
        explicit VoidType(TypeContext &context);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsVoid() const override;
    };

    struct NullType final : Type
    {
        explicit NullType(TypeContext &context);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsNull() const override;
    };

    struct BooleanType final : Type
    {
        explicit BooleanType(TypeContext &context);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsBoolean() const override;
    };

    struct NumberType final : Type
    {
        explicit NumberType(TypeContext &context);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsNumber() const override;
    };

    struct StringType final : Type
    {
        explicit StringType(TypeContext &context);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsString() const override;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        ArrayType(TypeContext &context, TypePtr elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsArray() const override;

        TypePtr Elements;
    };

    /** { a: A, ..., b: B } */
    struct ObjectType final : Type
    {
        ObjectType(TypeContext &context, const std::map<std::string, TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsObject() const override;

        std::map<std::string, TypePtr> Elements;
    };

    /** [ A, ..., B ] */
    struct TupleType final : Type
    {
        TupleType(TypeContext &context, const std::vector<TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsTuple() const override;

        std::vector<TypePtr> Elements;
    };

    /** A | ... | B */
    struct UnionType final : Type
    {
        UnionType(TypeContext &context, const std::set<TypePtr> &elements);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsUnion() const override;

        std::set<TypePtr> Elements;
    };

    /** $(A, ..., B) => C, $!(A, ..., B) => C */
    struct FunctionType final : Type
    {
        FunctionType(TypeContext &context, const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        bool IsFunction() const override;

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
