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
        TokenType_Target,
        TokenType_Other,
        TokenType_Undefined,
    };

    struct Location
    {
        std::string Filename;
        unsigned Row;
        unsigned Col;
    };

    struct Token
    {
        TokenType Type;
        Location Where;
        std::string RawValue;
        std::string Value;
        IntegerT Integer;
        FloatT Float;
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
                {mcc::TokenType_Integer, "int"},
                {mcc::TokenType_Float, "float"},
                {mcc::TokenType_String, "string"},
                {mcc::TokenType_FormatString, "format string"},
                {mcc::TokenType_Target, "target"},
                {mcc::TokenType_Other, "other"},
                {mcc::TokenType_Undefined, "undefined"},
            };

            return formatter<string>::format(types.contains(type) ? types.at(type) : "<undefined>", ctx);
        }
    };
}
