#include <mcc/lex.hpp>

enum lex_state
{
    lex_state_none,
    lex_state_symbol,
    lex_state_int,
    lex_state_float,
    lex_state_string,
};

mcc::token_t mcc::lex(std::istream &stream)
{
    auto c = stream.get();
    auto state = lex_state_none;

    std::string value;

    while (c >= 0)
    {
        switch (state)
        {
            case lex_state_none:
                switch (c)
                {
                    case '\n':
                        return {token_type_eol};

                    case '(':
                    case ')':
                    case'{':
                    case'}':
                    case'[':
                    case']':
                    case ',':
                        return {token_type_operator, {static_cast<char>(c)}};

                    case '@':
                        value += static_cast<char>(c);
                        c = stream.get();
                        value += static_cast<char>(c);
                        return {token_type_symbol, value};

                    case '"':
                        c = stream.get();
                        state = lex_state_string;
                        break;

                    default:
                        if (std::isspace(c))
                        {
                            c = stream.get();
                            break;
                        }

                        if (std::isalpha(c) || c == '_')
                        {
                            state = lex_state_symbol;
                            break;
                        }

                        c = stream.get();
                        break;
                }
                break;

            case lex_state_symbol:
                if (!std::isalnum(c) && c != '_')
                {
                    stream.unget();
                    return {token_type_symbol, value};
                }

                value += static_cast<char>(c);
                c = stream.get();
                break;

            case lex_state_int:
                break;

            case lex_state_float:
                break;

            case lex_state_string:
                if (c == '"')
                    return {token_type_string, value};

                value += static_cast<char>(c);
                c = stream.get();
                break;
        }
    }

    return {token_type_eof};
}
