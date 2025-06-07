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
        TypePtr GetObject(const std::map<std::string, TypePtr> &elements);
        TypePtr GetTuple(const std::vector<TypePtr> &elements);
        TypePtr GetUnion(const std::set<TypePtr> &elements);
        TypePtr GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        TypePtr GetUnionOrSingle(const std::set<TypePtr> &elements);

        TypePtr SetNamed(const std::string &name, const TypePtr &type);
        TypePtr GetNamed(const std::string &name) const;

    private:
        std::map<std::string, TypePtr> m_Named;

        TypePtr m_Void, m_Null, m_Boolean, m_Number, m_String;
        std::map<TypePtr, std::shared_ptr<struct ArrayType>> m_Array;
        std::map<std::map<std::string, TypePtr>, std::shared_ptr<struct ObjectType>> m_Struct;
        std::map<std::vector<TypePtr>, std::shared_ptr<struct TupleType>> m_Tuple;
        std::map<std::set<TypePtr>, std::shared_ptr<struct UnionType>> m_Union;
        std::map<std::vector<TypePtr>, std::map<TypePtr, std::map<bool, std::shared_ptr<struct FunctionType>>>>
        m_Function;
    };

    struct Type
    {
        explicit Type(TypeContext &context);
        virtual ~Type() = default;

        [[nodiscard]] virtual std::string String() const = 0;
        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] virtual ConstantPtr GetNull(const SourceLocation &where) const = 0;

        [[nodiscard]] virtual bool IsVoid() const;
        [[nodiscard]] virtual bool IsNull() const;
        [[nodiscard]] virtual bool IsBoolean() const;
        [[nodiscard]] virtual bool IsNumber() const;
        [[nodiscard]] virtual bool IsString() const;
        [[nodiscard]] virtual bool IsArray() const;
        [[nodiscard]] virtual bool IsObject() const;
        [[nodiscard]] virtual bool IsTuple() const;
        [[nodiscard]] virtual bool IsUnion() const;
        [[nodiscard]] virtual bool IsFunction() const;

        TypeContext &Context;
        std::weak_ptr<Type> Self;
    };

    struct VoidType final : Type
    {
        explicit VoidType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsVoid() const override;
    };

    struct NullType final : Type
    {
        explicit NullType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsNull() const override;
    };

    struct BooleanType final : Type
    {
        explicit BooleanType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsBoolean() const override;
    };

    struct NumberType final : Type
    {
        explicit NumberType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsNumber() const override;
    };

    struct StringType final : Type
    {
        explicit StringType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsString() const override;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        ArrayType(TypeContext &context, TypePtr elements);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsArray() const override;

        TypePtr Elements;
    };

    /** { a: A, ..., b: B } */
    struct ObjectType final : Type
    {
        ObjectType(TypeContext &context, const std::map<std::string, TypePtr> &elements);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsObject() const override;

        std::map<std::string, TypePtr> Elements;
    };

    /** [ A, ..., B ] */
    struct TupleType final : Type
    {
        TupleType(TypeContext &context, const std::vector<TypePtr> &elements);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsTuple() const override;

        std::vector<TypePtr> Elements;
    };

    /** A | ... | B */
    struct UnionType final : Type
    {
        UnionType(TypeContext &context, const std::set<TypePtr> &elements);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsUnion() const override;

        std::set<TypePtr> Elements;
    };

    /** $(A, ..., B) => C, $!(A, ..., B) => C */
    struct FunctionType final : Type
    {
        FunctionType(TypeContext &context, const std::vector<TypePtr> &parameters, TypePtr result, bool throws);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;

        [[nodiscard]] bool IsFunction() const override;

        std::vector<TypePtr> Parameters;
        TypePtr Result;
        bool Throws;
    };
}

template<>
struct std::formatter<mcc::TypePtr> final : std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(const mcc::TypePtr &type, FormatContext &ctx) const
    {
        return std::formatter<std::string>::format(type->String(), ctx);
    }
};
