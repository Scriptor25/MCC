#pragma once

#include <format>
#include <map>
#include <string>

namespace mcc
{
    enum FrameFlagE
    {
        FrameFlag_RequireHead = 0b001,
        FrameFlag_RequireTail = 0b010,
        FrameFlag_RequireLandingPad = 0b100,
    };

    enum ResultTypeE
    {
        ResultType_Value,
        ResultType_Storage,
        ResultType_Argument,
    };

    enum ComparatorE
    {
        Comparator_None,
        Comparator_LT,
        Comparator_GT,
        Comparator_LE,
        Comparator_GE,
        Comparator_EQ,
    };

    enum OperatorE
    {
        Operator_None,
        Operator_Add,
        Operator_Sub,
        Operator_Mul,
        Operator_Div,
        Operator_Rem,
    };

    enum ArrayOperationE
    {
        ArrayOperation_Append,
        ArrayOperation_Prepend,
        ArrayOperation_Insert,
    };

    inline const char *ToString(const ResultTypeE enum_)
    {
        static const std::map<ResultTypeE, const char *> map
        {
            {ResultType_Value, "value"},
            {ResultType_Storage, "storage"},
            {ResultType_Argument, "argument"},
        };

        return map.at(enum_);
    }

    inline const char *ToString(const ComparatorE enum_)
    {
        static const std::map<ComparatorE, const char *> map
        {
            {Comparator_None, "none"},
            {Comparator_LT, "lt"},
            {Comparator_GT, "gt"},
            {Comparator_LE, "le"},
            {Comparator_GE, "ge"},
            {Comparator_EQ, "eq"},
        };

        return map.at(enum_);
    }

    inline const char *ToString(const OperatorE enum_)
    {
        static const std::map<OperatorE, const char *> map
        {
            {Operator_None, "none"},
            {Operator_Add, "add"},
            {Operator_Sub, "sub"},
            {Operator_Mul, "mul"},
            {Operator_Div, "div"},
            {Operator_Rem, "rem"},
        };

        return map.at(enum_);
    }
}

namespace std
{
    template<>
    struct formatter<mcc::ResultTypeE> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::ResultTypeE &type, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(type), ctx);
        }
    };

    template<>
    struct formatter<mcc::ComparatorE> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::ComparatorE &comparator_, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(comparator_), ctx);
        }
    };

    template<>
    struct formatter<mcc::OperatorE> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::OperatorE &operator_, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(operator_), ctx);
        }
    };
}
