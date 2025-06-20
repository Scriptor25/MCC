#pragma once

#include <format>
#include <map>
#include <string>

namespace mcc
{
    enum E_ArrayOperation
    {
        ArrayOperation_Append,
        ArrayOperation_Prepend,
        ArrayOperation_Insert,
    };

    enum E_Comparator
    {
        Comparator_None,
        Comparator_LT,
        Comparator_GT,
        Comparator_LE,
        Comparator_GE,
        Comparator_EQ,
    };

    enum E_Declarator
    {
        Declarator_Let,
        Declarator_Const,
        Declarator_ConstExpr,
    };

    enum E_FrameFlag
    {
        FrameFlag_RequireHead       = 0b001,
        FrameFlag_RequireTail       = 0b010,
        FrameFlag_RequireLandingPad = 0b100,
    };

    enum E_Operator
    {
        Operator_Add,
        Operator_Sub,
        Operator_Mul,
        Operator_Div,
        Operator_Rem,
    };

    enum E_ReferenceType
    {
        ReferenceType_Block,
        ReferenceType_Entity,
        ReferenceType_Storage,
    };

    enum E_ResultType
    {
        ResultType_Value,
        ResultType_Reference,
        ResultType_Argument,
    };

    enum E_FieldType
    {
        FieldType_Value,
        FieldType_ImmutableReference,
        FieldType_MutableReference,
    };

    inline std::optional<E_Declarator> ToDeclarator(const std::string_view &string_)
    {
        static const std::map<std::string_view, E_Declarator> map
        {
            { "let", Declarator_Let },
            { "const", Declarator_Const },
            { "constexpr", Declarator_ConstExpr },
        };

        if (!map.contains(string_))
            return std::nullopt;

        return map.at(string_);
    }

    inline std::optional<E_Operator> ToOperator(const std::string_view &string_)
    {
        static const std::map<std::string_view, E_Operator> map
        {
            { "+", Operator_Add },
            { "-", Operator_Sub },
            { "*", Operator_Mul },
            { "/", Operator_Div },
            { "%", Operator_Rem },
        };

        if (!map.contains(string_))
            return std::nullopt;

        return map.at(string_);
    }

    inline std::optional<E_ReferenceType> ToReferenceType(const std::string_view &string_)
    {
        static const std::map<std::string_view, E_ReferenceType> map
        {
            { "block", ReferenceType_Block },
            { "entity", ReferenceType_Entity },
            { "storage", ReferenceType_Storage },
        };

        if (!map.contains(string_))
            return std::nullopt;

        return map.at(string_);
    }

    inline const char *ToString(const E_Comparator enum_)
    {
        static const std::map<E_Comparator, const char *> map
        {
            { Comparator_None, "none" },
            { Comparator_LT, "lt" },
            { Comparator_GT, "gt" },
            { Comparator_LE, "le" },
            { Comparator_GE, "ge" },
            { Comparator_EQ, "eq" },
        };

        return map.at(enum_);
    }

    inline const char *ToString(const E_Declarator enum_)
    {
        static const std::map<E_Declarator, const char *> map
        {
            { Declarator_Let, "let" },
            { Declarator_Const, "const" },
            { Declarator_ConstExpr, "constexpr" },
        };

        return map.at(enum_);
    }

    inline const char *ToString(const E_Operator enum_)
    {
        static const std::map<E_Operator, const char *> map
        {
            { Operator_Add, "+" },
            { Operator_Sub, "-" },
            { Operator_Mul, "*" },
            { Operator_Div, "/" },
            { Operator_Rem, "%" },
        };

        return map.at(enum_);
    }

    inline const char *ToString(const E_ReferenceType enum_)
    {
        static const std::map<E_ReferenceType, const char *> map
        {
            { ReferenceType_Block, "block" },
            { ReferenceType_Entity, "entity" },
            { ReferenceType_Storage, "storage" },
        };

        return map.at(enum_);
    }

    inline const char *ToString(const E_ResultType enum_)
    {
        static const std::map<E_ResultType, const char *> map
        {
            { ResultType_Value, "value" },
            { ResultType_Reference, "reference" },
            { ResultType_Argument, "argument" },
        };

        return map.at(enum_);
    }

    inline const char *ToString(const E_FieldType enum_)
    {
        static const std::map<E_FieldType, const char *> map
        {
            { FieldType_Value, "value" },
            { FieldType_ImmutableReference, "immutable_reference" },
            { FieldType_MutableReference, "mutable_reference" },
        };

        return map.at(enum_);
    }
}

namespace std
{
    template<>
    struct formatter<mcc::E_Comparator> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_Comparator &comparator_, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(comparator_), ctx);
        }
    };

    template<>
    struct formatter<mcc::E_Declarator> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_Declarator &declarator, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(declarator), ctx);
        }
    };

    template<>
    struct formatter<mcc::E_Operator> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_Operator &operator_, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(operator_), ctx);
        }
    };

    template<>
    struct formatter<mcc::E_ReferenceType> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_ReferenceType &type, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(type), ctx);
        }
    };

    template<>
    struct formatter<mcc::E_ResultType> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_ResultType &type, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(type), ctx);
        }
    };

    template<>
    struct formatter<mcc::E_FieldType> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::E_FieldType &type, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(type), ctx);
        }
    };
}
