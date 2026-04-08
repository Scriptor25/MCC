#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    enum class TokenType
    {
        EoF,
        Symbol,
        Operator,
        Number,
        String,
        FormatString,
        Other,
        Undefined,
    };

    struct Token
    {
        TokenType Type;
        SourceLocation Where;
        std::string Raw;
        std::string Value;
        IndexT Number = 0;
    };
}

namespace std
{
    template<>
    struct formatter<mcc::TokenType> : formatter<string>
    {
        template<typename FormatContext>
        auto format(
                const mcc::TokenType &val,
                FormatContext &ctx) const
        {
            static const std::map<mcc::TokenType, std::string> map{
                {          mcc::TokenType::EoF,           "eof" },
                {       mcc::TokenType::Symbol,        "symbol" },
                {     mcc::TokenType::Operator,      "operator" },
                {       mcc::TokenType::Number,        "number" },
                {       mcc::TokenType::String,        "string" },
                { mcc::TokenType::FormatString, "format string" },
                {        mcc::TokenType::Other,         "other" },
                {    mcc::TokenType::Undefined,     "undefined" },
            };

            if (auto it = map.find(val); it != map.end())
                return formatter<string>::format(it->second, ctx);

            return formatter<string>::format("<undefined>", ctx);
        }
    };
}
