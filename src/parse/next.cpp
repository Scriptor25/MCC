#include <mcc/parse.hpp>

#include <set>

mcc::Token &mcc::Parser::Next()
{
    static const std::map<std::string, std::set<int>> operator_map{
        { "=",      { '=', '>' } },
        { "<",           { '=' } },
        { ">",           { '=' } },
        { "+",      { '=', '+' } },
        { "-", { '=', '-', '>' } },
        { "*",           { '=' } },
        { "/",           { '=' } },
        { "%",           { '=' } },
    };

    enum class LexState
    {
        None,
        Symbol,
        Number,
        String,
        Operator,
        Comment,
    };

    auto state = LexState::None;
    auto where = m_Where;

    std::string raw;
    std::string value;

    auto formatted = false;

    while (m_Buf >= 0)
    {
        switch (state)
        {
        case LexState::None:
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
            case '@':
            case '.':
            case '&':
            case '|':
                where = m_Where;
                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                return m_Token = {
                    .Type  = TokenType::Other,
                    .Where = std::move(where),
                    .Raw   = std::move(raw),
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
                state = LexState::Operator;
                Get();
                break;

            case '`':
                formatted = true;
            case '"':
                raw += static_cast<char>(m_Buf);
                where = m_Where;
                state = LexState::String;
                Get();
                break;

            default:
                if (std::isspace(m_Buf))
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                    break;
                }

                if (std::isdigit(m_Buf))
                {
                    where = m_Where;
                    state = LexState::Number;
                    break;
                }

                if (std::isalpha(m_Buf) || m_Buf == '_')
                {
                    where = m_Where;
                    state = LexState::Symbol;
                    break;
                }

                where = m_Where;
                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                return m_Token = {
                    .Type  = TokenType::Undefined,
                    .Where = std::move(where),
                    .Raw   = std::move(raw),
                    .Value = std::move(value),
                };
            }
            break;

        case LexState::Symbol:
            if (!std::isalnum(m_Buf) && m_Buf != '_')
                return m_Token = {
                    .Type  = TokenType::Symbol,
                    .Where = std::move(where),
                    .Raw   = std::move(raw),
                    .Value = std::move(value),
                };

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            Get();
            break;

        case LexState::Number:
            if (!std::isdigit(m_Buf))
            {
                return m_Token = {
                    .Type   = TokenType::Number,
                    .Where  = std::move(where),
                    .Raw    = std::move(raw),
                    .Value  = value,
                    .Number = std::stoull(value),
                };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            Get();
            break;

        case LexState::String:
            if (m_Buf == (formatted ? '`' : '"'))
            {
                raw += static_cast<char>(m_Buf);
                Get();
                return m_Token = {
                    .Type  = formatted ? TokenType::FormatString : TokenType::String,
                    .Where = std::move(where),
                    .Raw   = std::move(raw),
                    .Value = std::move(value),
                };
            }

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            Get();
            break;

        case LexState::Operator:
            if (value == "/" && m_Buf == '*')
            {
                raw += static_cast<char>(m_Buf);
                state = LexState::Comment;
                Get();
                break;
            }

            if (!operator_map.contains(value) || !operator_map.at(value).contains(m_Buf))
                return m_Token = {
                    .Type  = TokenType::Operator,
                    .Where = std::move(where),
                    .Raw   = std::move(raw),
                    .Value = std::move(value),
                };

            raw += static_cast<char>(m_Buf);
            value += static_cast<char>(m_Buf);
            Get();
            break;

        case LexState::Comment:
            if (m_Buf == '*')
            {
                raw += static_cast<char>(m_Buf);
                Get();
                if (m_Buf == '/')
                {
                    raw += static_cast<char>(m_Buf);
                    value.clear();
                    state = LexState::None;
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
        .Type  = TokenType::EoF,
        .Where = std::move(where),
    };
}
