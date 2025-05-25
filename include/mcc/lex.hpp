#pragma once

#include <format>
#include <string>
#include <mcc/common.hpp>

namespace mcc
{
    enum TokenType
    {
        TokenType_EOF,
        TokenType_Symbol,
        TokenType_Operator,
        TokenType_Integer,
        TokenType_Float,
        TokenType_String,
        TokenType_FormatString,
        TokenType_Other,
        TokenType_Undefined,
    };

    struct Token
    {
        TokenType Type;
        SourceLocation Where;
        std::string Raw;
        std::string Value;
        IntegerT Integer = 0;
        FloatT Float = 0.0;
        std::pair<std::optional<IntegerT>, std::optional<IntegerT>> Range;
    };
}

namespace std
{
    template<>
    struct formatter<mcc::TokenType> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::TokenType &type, FormatContext &ctx) const
        {
            static const std::map<mcc::TokenType, std::string> types
            {
                {mcc::TokenType_EOF, "eof"},
                {mcc::TokenType_Symbol, "symbol"},
                {mcc::TokenType_Operator, "operator"},
                {mcc::TokenType_Integer, "integer"},
                {mcc::TokenType_Float, "float"},
                {mcc::TokenType_String, "string"},
                {mcc::TokenType_FormatString, "format string"},
                {mcc::TokenType_Other, "other"},
                {mcc::TokenType_Undefined, "undefined"},
            };

            return formatter<string>::format(types.contains(type) ? types.at(type) : "<undefined>", ctx);
        }
    };
}
