#pragma once

#include <string>

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
        long long Integer;
        double Float;
    };
}
