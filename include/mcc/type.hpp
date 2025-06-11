#pragma once

#include <map>
#include <set>
#include <vector>
#include <mcc/common.hpp>

namespace mcc
{
    bool SameOrSpecial(const TypePtr &a, const TypePtr &b);

    class TypeContext
    {
    public:
        TypePtr GetVoid();
        TypePtr GetNumber();
        TypePtr GetString();
        TypePtr GetArray(const TypePtr &elements);
        TypePtr GetObject(const std::map<std::string, TypePtr> &elements);
        TypePtr GetTuple(const std::vector<TypePtr> &elements);
        TypePtr GetUnion(const std::set<TypePtr> &elements);
        TypePtr GetFunction(const std::vector<TypePtr> &parameters, const TypePtr &result, bool throws);

        TypePtr GetAnyArray();
        TypePtr GetAnyObject();
        TypePtr GetAnyFunction();

        TypePtr GetUnionOrSingle(const std::set<TypePtr> &elements);

        TypePtr SetNamed(const std::string &name, const TypePtr &type);
        [[nodiscard]] TypePtr GetNamed(const std::string &name) const;

    private:
        std::map<std::string, TypePtr> m_Named;

        TypePtr m_Void, m_Number, m_String, m_AnyArray, m_AnyObject,  m_AnyFunction;
        std::map<TypePtr, TypePtr> m_Array;
        std::map<std::map<std::string, TypePtr>, TypePtr> m_Struct;
        std::map<std::vector<TypePtr>, TypePtr> m_Tuple;
        std::map<std::set<TypePtr>, TypePtr> m_Union;
        std::map<std::vector<TypePtr>, std::map<TypePtr, std::map<bool, TypePtr>>> m_Function;
    };

    struct Type
    {
        explicit Type(TypeContext &context);
        virtual ~Type() = default;

        [[nodiscard]] virtual std::string String() const = 0;
        virtual std::ostream &Print(std::ostream &stream) const = 0;

        [[nodiscard]] virtual ConstantPtr GetNull(const SourceLocation &where) const = 0;
        [[nodiscard]] virtual bool HasSpecial(const TypePtr &other) const = 0;

        [[nodiscard]] virtual bool IsAny() const;
        [[nodiscard]] virtual bool IsVoid() const;
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
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsVoid() const override;
    };

    struct NumberType final : Type
    {
        explicit NumberType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsNumber() const override;
    };

    struct StringType final : Type
    {
        explicit StringType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsString() const override;
    };

    /** A[] */
    struct ArrayType final : Type
    {
        ArrayType(TypeContext &context, TypePtr elements);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

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
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

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
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

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
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

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
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsFunction() const override;

        std::vector<TypePtr> Parameters;
        TypePtr Result;
        bool Throws;
    };

    struct AnyArrayType final : Type
    {
        explicit AnyArrayType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsAny() const override;
        [[nodiscard]] bool IsArray() const override;
    };

    struct AnyObjectType final : Type
    {
        explicit AnyObjectType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsAny() const override;
        [[nodiscard]] bool IsObject() const override;
    };

    struct AnyFunctionType final : Type
    {
        explicit AnyFunctionType(TypeContext &context);

        [[nodiscard]] std::string String() const override;
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] ConstantPtr GetNull(const SourceLocation &where) const override;
        [[nodiscard]] bool HasSpecial(const TypePtr &other) const override;

        [[nodiscard]] bool IsAny() const override;
        [[nodiscard]] bool IsFunction() const override;
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
