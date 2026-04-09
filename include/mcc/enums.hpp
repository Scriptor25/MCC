#pragma once

#include <format>
#include <map>
#include <optional>
#include <string_view>

namespace mcc
{
    enum class ArrayOperation_
    {
        Append,
        Prepend,
        Insert,
    };

    enum class Comparator_
    {
        LT,
        GT,
        LE,
        GE,
        EQ,
    };

    enum class Declarator_
    {
        Let,
        Const,
        ConstExpr,
    };

    enum FrameFlag_
    {
        FrameFlag_RequireHead       = 0b001,
        FrameFlag_RequireTail       = 0b010,
        FrameFlag_RequireLandingPad = 0b100,
    };

    enum class Operator_
    {
        Add,
        Sub,
        Mul,
        Div,
        Rem,
    };

    enum class ReferenceType_
    {
        Block,
        Entity,
        Storage,
    };

    enum class ResultType_
    {
        Value,
        Reference,
        Argument,
    };

    enum class FieldType_
    {
        Value,
        ImmutableReference,
        MutableReference,
    };

    inline std::optional<Declarator_> ToDeclarator(const std::string_view &str)
    {
        static const std::map<std::string_view, Declarator_> map = {
            {       "let",       Declarator_::Let },
            {     "const",     Declarator_::Const },
            { "constexpr", Declarator_::ConstExpr },
        };

        if (const auto it = map.find(str); it != map.end())
            return it->second;

        return std::nullopt;
    }

    inline std::optional<Operator_> ToOperator(const std::string_view &str)
    {
        static const std::map<std::string_view, Operator_> map = {
            { "+", Operator_::Add },
            { "-", Operator_::Sub },
            { "*", Operator_::Mul },
            { "/", Operator_::Div },
            { "%", Operator_::Rem },
        };

        if (const auto it = map.find(str); it != map.end())
            return it->second;

        return std::nullopt;
    }

    inline std::optional<ReferenceType_> ToReferenceType(const std::string_view &str)
    {
        static const std::map<std::string_view, ReferenceType_> map = {
            {   "block",   ReferenceType_::Block },
            {  "entity",  ReferenceType_::Entity },
            { "storage", ReferenceType_::Storage },
        };

        if (const auto it = map.find(str); it != map.end())
            return it->second;

        return std::nullopt;
    }

    inline const char *ToString(const Comparator_ val)
    {
        static const std::map<Comparator_, const char *> map = {
            { Comparator_::LT, "lt" },
            { Comparator_::GT, "gt" },
            { Comparator_::LE, "le" },
            { Comparator_::GE, "ge" },
            { Comparator_::EQ, "eq" },
        };

        return map.at(val);
    }

    inline const char *ToString(const Declarator_ val)
    {
        static const std::map<Declarator_, const char *> map = {
            {       Declarator_::Let,       "let" },
            {     Declarator_::Const,     "const" },
            { Declarator_::ConstExpr, "constexpr" },
        };

        return map.at(val);
    }

    inline const char *ToString(const Operator_ val)
    {
        static const std::map<Operator_, const char *> map = {
            { Operator_::Add, "+" },
            { Operator_::Sub, "-" },
            { Operator_::Mul, "*" },
            { Operator_::Div, "/" },
            { Operator_::Rem, "%" },
        };

        return map.at(val);
    }

    inline const char *ToString(const ReferenceType_ val)
    {
        static const std::map<ReferenceType_, const char *> map = {
            {   ReferenceType_::Block,   "block" },
            {  ReferenceType_::Entity,  "entity" },
            { ReferenceType_::Storage, "storage" },
        };

        return map.at(val);
    }

    inline const char *ToString(const ResultType_ val)
    {
        static const std::map<ResultType_, const char *> map = {
            {     ResultType_::Value,     "value" },
            { ResultType_::Reference, "reference" },
            {  ResultType_::Argument,  "argument" },
        };

        return map.at(val);
    }

    inline const char *ToString(const FieldType_ val)
    {
        static const std::map<FieldType_, const char *> map = {
            {              FieldType_::Value,               "value" },
            { FieldType_::ImmutableReference, "immutable_reference" },
            {   FieldType_::MutableReference,   "mutable_reference" },
        };

        return map.at(val);
    }
}

namespace std
{
    template<>
    struct formatter<mcc::Comparator_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::Comparator_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };

    template<>
    struct formatter<mcc::Declarator_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::Declarator_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };

    template<>
    struct formatter<mcc::Operator_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::Operator_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };

    template<>
    struct formatter<mcc::ReferenceType_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::ReferenceType_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };

    template<>
    struct formatter<mcc::ResultType_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::ResultType_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };

    template<>
    struct formatter<mcc::FieldType_> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::FieldType_ &val,
                FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(val), ctx);
        }
    };
}
