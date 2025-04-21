#pragma once

#include <format>
#include <istream>
#include <map>
#include <string>

namespace mcc
{
    enum token_type
    {
        token_type_eof,
        token_type_eol,
        token_type_symbol,
        token_type_operator,
        token_type_int,
        token_type_float,
        token_type_string,
    };

    struct token_t
    {
        token_type type;
        std::string value;
    };

    token_t lex(std::istream &stream);
}

namespace std
{
    template<>
    struct formatter<mcc::token_type> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::token_type &type, FormatContext &ctx) const
        {
            static const std::map<mcc::token_type, std::string> types
            {
                {mcc::token_type_eof, "eof"},
                {mcc::token_type_eol, "eol"},
                {mcc::token_type_symbol, "symbol"},
                {mcc::token_type_operator, "operator"},
                {mcc::token_type_int, "int"},
                {mcc::token_type_float, "float"},
                {mcc::token_type_string, "string"},
            };

            return formatter<string>::format(types.at(type), ctx);
        }
    };
}
