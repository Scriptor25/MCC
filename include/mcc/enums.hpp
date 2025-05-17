#pragma once

#include <format>
#include <map>
#include <ostream>
#include <string>

namespace mcc
{
    enum TypeID
    {
        TypeID_Boolean,
        TypeID_Number,
        TypeID_String,
        TypeID_Array,
        TypeID_Object,
    };

    enum FrameFlagE
    {
        FrameFlag_RequireHead = 0b001,
        FrameFlag_RequireTail = 0b010,
        FrameFlag_RequireLandingPad = 0b100,
    };

    enum ResultTypeE
    {
        ResultType_None,
        ResultType_Value,
        ResultType_Storage,
        ResultType_Score,
    };

    enum TargetSelectorE
    {
        TargetSelector_P,
        TargetSelector_R,
        TargetSelector_A,
        TargetSelector_E,
        TargetSelector_S,
    };

    enum GameModeE
    {
        GameMode_None,
        GameMode_Survival,
        GameMode_Creative,
        GameMode_Adventure,
        GameMode_Hardcore,
        GameMode_Spectator,
    };

    enum SortOrderE
    {
        SortOrder_None,
        SortOrder_Nearest,
        SortOrder_Furthest,
        SortOrder_Random,
        SortOrder_Arbitrary,
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

    enum AllocationTypeE
    {
        AllocationType_Value,
        AllocationType_Array,
        AllocationType_Object,
    };

    enum ArrayOperationE
    {
        ArrayOperation_Append,
        ArrayOperation_Prepend,
        ArrayOperation_Insert,
        ArrayOperation_Extract,
    };

    enum OffsetTypeE
    {
        OffsetType_Relative,
        OffsetType_Local,
    };

    inline TargetSelectorE ToTargetSelector(const std::string_view &string)
    {
        static const std::map<std::string_view, TargetSelectorE> map
        {
            {"p", TargetSelector_P},
            {"r", TargetSelector_R},
            {"a", TargetSelector_A},
            {"e", TargetSelector_E},
            {"s", TargetSelector_S},
        };

        return map.at(string);
    }

    inline OffsetTypeE ToOffsetType(const std::string_view &string)
    {
        static const std::map<std::string_view, OffsetTypeE> map
        {
            {"~", OffsetType_Relative},
            {"^", OffsetType_Local},
        };

        return map.at(string);
    }

    inline const char *ToString(const ResultTypeE enum_)
    {
        static const std::map<ResultTypeE, const char *> map
        {
            {ResultType_None, "none"},
            {ResultType_Value, "value"},
            {ResultType_Storage, "storage"},
            {ResultType_Score, "score"},
        };

        return map.at(enum_);
    }

    inline const char *ToString(const TargetSelectorE enum_)
    {
        static const std::map<TargetSelectorE, const char *> map
        {
            {TargetSelector_P, "p"},
            {TargetSelector_R, "r"},
            {TargetSelector_A, "a"},
            {TargetSelector_E, "e"},
            {TargetSelector_S, "s"},
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

    inline const char *ToString(const OffsetTypeE enum_)
    {
        static const std::map<OffsetTypeE, const char *> map
        {
            {OffsetType_Relative, "~"},
            {OffsetType_Local, "^"},
        };

        return map.at(enum_);
    }

    inline std::ostream &operator<<(std::ostream &stream, const TargetSelectorE &e)
    {
        return stream << ToString(e);
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
