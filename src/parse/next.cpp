#include <set>
#include <mcc/parse.hpp>

mcc::Token &mcc::Parser::Next()
{
    static const std::map<std::string, std::set<int>> operator_map
    {
        {"=", {'='}},
        {"!", {'='}},
        {"<", {'='}},
        {">", {'='}},
    };

    enum LexState
    {
        LexState_None,
        LexState_Symbol,
        LexState_Int,
        LexState_Float,
        LexState_String,
        LexState_Operator,
        LexState_Comment,
    };

    auto state = LexState_None;
    auto token_location = m_Location;

    std::string raw_value;
    std::string value;

    auto formatted = false;

    while (m_Buf >= 0)
    {
        switch (state)
        {
            case LexState_None:
                switch (m_Buf)
                {
                    case '(':
                    case ')':
                    case '{':
                    case '}':
                    case '[':
                    case ']':
                    case '.':
                    case ':':
                    case ',':
                    case ';':
                    case '~':
                    case '^':
                    case '#':
                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {TokenType_Other, token_location, raw_value, value, 0, 0};

                    case '!':
                    case '=':
                    case '<':
                    case '>':
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '%':
                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        state = LexState_Operator;
                        break;

                    case '@':
                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        Get();
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {TokenType_Target, token_location, raw_value, value, 0, 0};

                    case '`':
                        formatted = true;
                    case '"':
                        raw_value += static_cast<char>(m_Buf);
                        token_location = m_Location;
                        state = LexState_String;
                        Get();
                        break;

                    default:
                        if (std::isspace(m_Buf))
                        {
                            raw_value += static_cast<char>(m_Buf);
                            Get();
                            break;
                        }

                        if (std::isdigit(m_Buf))
                        {
                            token_location = m_Location;
                            state = LexState_Int;
                            break;
                        }

                        if (std::isalpha(m_Buf) || m_Buf == '_')
                        {
                            token_location = m_Location;
                            state = LexState_Symbol;
                            break;
                        }

                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {TokenType_Undefined, token_location, raw_value, value, 0, 0};
                }
                break;

            case LexState_Symbol:
                if (!std::isalnum(m_Buf) && m_Buf != '_')
                {
                    return m_Token = {TokenType_Symbol, token_location, raw_value, value, 0, 0};
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Int:
                if (m_Buf == '.')
                {
                    raw_value += static_cast<char>(m_Buf);
                    value += static_cast<char>(m_Buf);
                    Get();
                    state = LexState_Float;
                    break;
                }

                if (!std::isdigit(m_Buf))
                {
                    return m_Token = {TokenType_Integer, token_location, raw_value, value, std::stoll(value), 0};
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Float:
                if (!std::isdigit(m_Buf))
                {
                    return m_Token = {TokenType_Float, token_location, raw_value, value, 0, std::stold(value)};
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_String:
                if (m_Buf == (formatted ? '`' : '"'))
                {
                    raw_value += static_cast<char>(m_Buf);
                    Get();
                    return m_Token = {
                               formatted
                                   ? TokenType_FormatString
                                   : TokenType_String,
                               token_location,
                               raw_value,
                               value,
                               0,
                               0,
                           };
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Operator:
                if (value == "/" && m_Buf == '*')
                {
                    Get();
                    state = LexState_Comment;
                    break;
                }

                if (!operator_map.contains(value) || !operator_map.at(value).contains(m_Buf))
                {
                    return m_Token = {TokenType_Operator, token_location, raw_value, value, 0, 0};
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Comment:
                if (m_Buf == '*')
                {
                    Get();
                    if (m_Buf == '/')
                    {
                        value.clear();
                        raw_value.clear();
                        Get();
                        state = LexState_None;
                        break;
                    }
                }
                else
                {
                    Get();
                }
                break;
        }
    }

    return m_Token = {TokenType_EOF, token_location, {}, {}, 0, 0};
}
