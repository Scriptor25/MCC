#pragma once

#include <format>
#include <map>
#include <vector>
#include <mcc/lex.hpp>

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

            return formatter<string>::format(types.contains(type) ? types.at(type) : "?", ctx);
        }
    };

    template<typename T>
    struct formatter<vector<T>> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const vector<T> &elements, FormatContext &ctx) const
        {
            string s = "[ ";

            for (unsigned i = 0; i < elements.size(); ++i)
            {
                if (i > 0)
                    s += ", ";
                s += std::format("{}", elements[i]);
            }

            s += " ]";

            return formatter<string>::format(s, ctx);
        }
    };
}
