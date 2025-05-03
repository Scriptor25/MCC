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
        LexState_Integer,
        LexState_String,
        LexState_Operator,
        LexState_Comment,
    };

    auto state = LexState_None;
    auto token_location = m_Location;

    std::string raw_value;
    std::string value;

    auto formatted = false;
    unsigned decimals = 0;

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
                        return m_Token = {
                                   .Type = TokenType_Other,
                                   .Where = token_location,
                                   .RawValue = raw_value,
                                   .Value = value
                               };

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
                        state = LexState_Operator;
                        Get();
                        break;

                    case '@':
                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        Get();
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {
                                   .Type = TokenType_Target,
                                   .Where = token_location,
                                   .RawValue = raw_value,
                                   .Value = value
                               };

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

                        if (std::isdigit(m_Buf) || m_Buf == '.')
                        {
                            token_location = m_Location;
                            state = LexState_Integer;
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
                        return m_Token = {
                                   .Type = TokenType_Undefined,
                                   .Where = token_location,
                                   .RawValue = raw_value,
                                   .Value = value
                               };
                }
                break;

            case LexState_Symbol:
                if (!std::isalnum(m_Buf) && m_Buf != '_')
                    return m_Token = {
                               .Type = TokenType_Symbol,
                               .Where = token_location,
                               .RawValue = raw_value,
                               .Value = value
                           };

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Integer:
                if (m_Buf == '.')
                {
                    decimals++;
                }
                else if (!std::isdigit(m_Buf))
                {
                    switch (decimals)
                    {
                        case 0:
                            return m_Token = {
                                       .Type = TokenType_Integer,
                                       .Where = token_location,
                                       .RawValue = raw_value,
                                       .Value = value,
                                       .Integer = std::stoll(value),
                                   };

                        case 1:
                            return m_Token = {
                                       .Type = TokenType_Float,
                                       .Where = token_location,
                                       .RawValue = raw_value,
                                       .Value = value,
                                       .Float = std::stold(value),
                                   };

                        case 2:
                        {
                            auto split = value.find("..");
                            return m_Token = {
                                       .Type = TokenType_Range,
                                       .Where = token_location,
                                       .RawValue = raw_value,
                                       .Value = value,
                                       .Range = {
                                           std::stoll(value.substr(0, split)),
                                           std::stoll(value.substr(split + 2)),
                                       },
                                   };
                        }

                        default:
                            throw std::runtime_error("TODO");
                    }
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
                               .Type = formatted
                                           ? TokenType_FormatString
                                           : TokenType_String,
                               .Where = token_location,
                               .RawValue = raw_value,
                               .Value = value,
                           };
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Operator:
                if (value == "/" && m_Buf == '*')
                {
                    raw_value += static_cast<char>(m_Buf);
                    state = LexState_Comment;
                    Get();
                    break;
                }

                if (!operator_map.contains(value) || !operator_map.at(value).contains(m_Buf))
                    return m_Token = {
                               .Type = TokenType_Operator,
                               .Where = token_location,
                               .RawValue = raw_value,
                               .Value = value
                           };

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Comment:
                if (m_Buf == '*')
                {
                    raw_value += static_cast<char>(m_Buf);
                    Get();
                    if (m_Buf == '/')
                    {
                        raw_value += static_cast<char>(m_Buf);
                        value.clear();
                        state = LexState_None;
                        Get();
                        break;
                    }
                }
                else
                {
                    raw_value += static_cast<char>(m_Buf);
                    Get();
                }
                break;
        }
    }

    return m_Token = {.Type = TokenType_EOF, .Where = token_location};
}
