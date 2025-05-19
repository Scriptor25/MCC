#include <set>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::Token &mcc::Parser::Next()
{
    static const std::map<std::string, std::set<int>> operator_map
    {
        {"=", {'='}},
        {"<", {'='}},
        {">", {'='}},
        {"+", {'=', '+'}},
        {"-", {'=', '-', '>'}},
        {"*", {'='}},
        {"/", {'='}},
        {"%", {'='}},
    };

    enum LexState
    {
        LexState_None,
        LexState_Symbol,
        LexState_Number,
        LexState_String,
        LexState_Operator,
        LexState_Comment,
    };

    auto state = LexState_None;
    auto where = m_Where;

    std::string raw;
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
                    case '$':
                    case '?':
                    case '!':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {
                                   .Type = TokenType_Other,
                                   .Where = std::move(where),
                                   .Raw = std::move(raw),
                                   .Value = std::move(value),
                               };

                    case '=':
                    case '<':
                    case '>':
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '%':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        state = LexState_Operator;
                        Get();
                        break;

                    case '@':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        Get();
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        if (m_Buf == '[')
                        {
                            raw += static_cast<char>(m_Buf);
                            Get();
                            return m_Token = {
                                       .Type = TokenType_TargetAttributes,
                                       .Where = std::move(where),
                                       .Raw = std::move(raw),
                                       .Value = std::move(value),
                                   };
                        }
                        return m_Token = {
                                   .Type = TokenType_Target,
                                   .Where = std::move(where),
                                   .Raw = std::move(raw),
                                   .Value = std::move(value),
                               };

                    case '`':
                        formatted = true;
                    case '"':
                        raw += static_cast<char>(m_Buf);
                        where = m_Where;
                        state = LexState_String;
                        Get();
                        break;

                    default:
                        if (std::isspace(m_Buf))
                        {
                            raw += static_cast<char>(m_Buf);
                            Get();
                            break;
                        }

                        if (std::isdigit(m_Buf) || m_Buf == '.')
                        {
                            where = m_Where;
                            state = LexState_Number;
                            break;
                        }

                        if (std::isalpha(m_Buf) || m_Buf == '_')
                        {
                            where = m_Where;
                            state = LexState_Symbol;
                            break;
                        }

                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {
                                   .Type = TokenType_Undefined,
                                   .Where = std::move(where),
                                   .Raw = std::move(raw),
                                   .Value = std::move(value),
                               };
                }
                break;

            case LexState_Symbol:
                if (!std::isalnum(m_Buf) && m_Buf != '.' && m_Buf != '_')
                    return m_Token = {
                               .Type = TokenType_Symbol,
                               .Where = std::move(where),
                               .Raw = std::move(raw),
                               .Value = std::move(value),
                           };

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Number:
                if (m_Buf == '.')
                {
                    decimals++;
                }
                else if (!std::isdigit(m_Buf))
                {
                    switch (decimals)
                    {
                        case 0:
                        {
                            auto integer = std::stoll(value);
                            return m_Token = {
                                       .Type = TokenType_Integer,
                                       .Where = std::move(where),
                                       .Raw = std::move(raw),
                                       .Value = std::move(value),
                                       .Integer = integer,
                                   };
                        }

                        case 1:
                        {
                            auto float_ = std::stold(value);
                            return m_Token = {
                                       .Type = TokenType_Float,
                                       .Where = std::move(where),
                                       .Raw = std::move(raw),
                                       .Value = std::move(value),
                                       .Float = float_,
                                   };
                        }

                        case 2:
                        {
                            auto split = value.find("..");
                            auto beg_string = value.substr(0, split);
                            auto end_string = value.substr(split + 2);
                            auto beg = beg_string.empty() ? std::nullopt : std::optional(std::stoll(beg_string));
                            auto end = end_string.empty() ? std::nullopt : std::optional(std::stoll(end_string));
                            return m_Token = {
                                       .Type = TokenType_Range,
                                       .Where = std::move(where),
                                       .Raw = std::move(raw),
                                       .Value = std::move(value),
                                       .Range = {beg, end},
                                   };
                        }

                        default:
                            Error(
                                m_Where,
                                "invalid number of decimal points in range token, must be either 0, 1 or 2, but is {}",
                                decimals);
                    }
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_String:
                if (m_Buf == (formatted ? '`' : '"'))
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                    return m_Token = {
                               .Type = formatted
                                           ? TokenType_FormatString
                                           : TokenType_String,
                               .Where = std::move(where),
                               .Raw = std::move(raw),
                               .Value = std::move(value),
                           };
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Operator:
                if (value == "/" && m_Buf == '*')
                {
                    raw += static_cast<char>(m_Buf);
                    state = LexState_Comment;
                    Get();
                    break;
                }

                if (!operator_map.contains(value) || !operator_map.at(value).contains(m_Buf))
                    return m_Token = {
                               .Type = TokenType_Operator,
                               .Where = std::move(where),
                               .Raw = std::move(raw),
                               .Value = std::move(value),
                           };

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Comment:
                if (m_Buf == '*')
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                    if (m_Buf == '/')
                    {
                        raw += static_cast<char>(m_Buf);
                        value.clear();
                        state = LexState_None;
                        Get();
                        break;
                    }
                }
                else
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                }
                break;
        }
    }

    return m_Token = {
               .Type = TokenType_EOF,
               .Where = std::move(where),
           };
}
