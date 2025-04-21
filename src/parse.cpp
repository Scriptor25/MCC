#include <istream>
#include <sstream>
#include <mcc/format.hpp>
#include <mcc/parse.hpp>

mcc::Parser::Parser(std::istream &stream, std::string filename)
    : m_Stream(stream),
      m_Location(std::move(filename), 1, 0)
{
    Get();
    Next();
}

mcc::Parser::operator bool() const
{
    return !m_Stream.eof();
}

mcc::StatementPtr mcc::Parser::operator()()
{
    return ParseStatement();
}

void mcc::Parser::Get()
{
    m_Buf = m_Stream.get();
    if (m_Buf == '\n')
    {
        m_Location.Col = 0;
        m_Location.Row++;
    }
    else
    {
        m_Location.Col++;
    }
}

mcc::Token &mcc::Parser::Next()
{
    enum LexState
    {
        LexState_None,
        LexState_Symbol,
        LexState_Int,
        LexState_Float,
        LexState_String,
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
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        m_Token = {TokenType_Other, m_Location, raw_value, value};
                        Get();
                        return m_Token;

                    case '=':
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '%':
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        m_Token = {TokenType_Operator, m_Location, raw_value, value};
                        Get();
                        return m_Token;

                    case '@':
                        token_location = m_Location;
                        raw_value += static_cast<char>(m_Buf);
                        Get();
                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {TokenType_Target, token_location, raw_value, value};

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

                        raw_value += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        m_Token = {TokenType_Undefined, m_Location, raw_value, value};
                        Get();
                        return m_Token;
                }
                break;

            case LexState_Symbol:
                if (!std::isalnum(m_Buf) && m_Buf != '_')
                    return m_Token = {TokenType_Symbol, token_location, raw_value, value};

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Int:
                if (!std::isdigit(m_Buf))
                    return m_Token = {TokenType_Integer, token_location, raw_value, value};

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case LexState_Float:
                break;

            case LexState_String:
                if (m_Buf == (formatted ? '`' : '"'))
                {
                    raw_value += static_cast<char>(m_Buf);
                    Get();
                    return m_Token = {
                               formatted ? TokenType_FormatString : TokenType_String,
                               token_location,
                               raw_value,
                               value
                           };
                }

                raw_value += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;
        }
    }

    return m_Token = {TokenType_EOF, token_location};
}

bool mcc::Parser::At(const TokenType type, const std::string &value) const
{
    if (m_Token.Type != type)
        return false;

    if (!value.empty() && m_Token.Value != value)
        return false;

    return true;
}

bool mcc::Parser::SkipIf(const TokenType type, const std::string &value)
{
    if (m_Token.Type != type)
        return false;

    if (!value.empty() && m_Token.Value != value)
        return false;

    Next();
    return true;
}

mcc::Token mcc::Parser::Skip()
{
    auto t = m_Token;
    Next();

    return t;
}

mcc::Token mcc::Parser::Expect(TokenType type, std::string value)
{
    if (m_Token.Type != type)
        throw std::runtime_error(
            std::format(
                "{}({},{}): expected {}, but is {}",
                m_Token.Where.Filename,
                m_Token.Where.Row,
                m_Token.Where.Col,
                type,
                m_Token.Type));

    if (!value.empty() && m_Token.Value != value)
        throw std::runtime_error(
            std::format(
                "{}({},{}): expected '{}', but is '{}'",
                m_Token.Where.Filename,
                m_Token.Where.Row,
                m_Token.Where.Col,
                value,
                m_Token.Value));

    return Skip();
}

mcc::Token mcc::Parser::ExpectEnum(std::vector<std::string> values)
{
    for (auto &value: values)
        if (At(TokenType_Symbol, value))
            return Skip();

    throw std::runtime_error(
        std::format(
            "{}({},{}): expected {}, but is '{}'",
            m_Token.Where.Filename,
            m_Token.Where.Row,
            m_Token.Where.Col,
            values,
            m_Token.Value));
}

mcc::ResourceLocation mcc::Parser::ParseResourceLocation()
{
    auto ns = std::string(SkipIf(TokenType_Other, ":") ? "" : "minecraft");
    auto id = Expect(TokenType_Symbol).Value;

    if (!ns.empty() && SkipIf(TokenType_Other, ":"))
    {
        ns = id;
        id = Expect(TokenType_Symbol).Value;
    }

    return {ns, id};
}

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (m_Token.Type == TokenType_EOF)
        return {};

    if (At(TokenType_Symbol, "namespace"))
        return ParseNamespaceStatement();
    if (At(TokenType_Symbol, "define"))
        return ParseDefineStatement();

    throw std::runtime_error(
        std::format(
            "{}({},{}): cannot parse {} '{}'",
            m_Token.Where.Filename,
            m_Token.Where.Row,
            m_Token.Where.Col,
            m_Token.Type,
            m_Token.Value));
}

mcc::StatementPtr mcc::Parser::ParseNamespaceStatement()
{
    Expect(TokenType_Symbol, "namespace");

    auto id = Expect(TokenType_Symbol).Value;

    return std::make_unique<NamespaceStatement>(id);
}

mcc::StatementPtr mcc::Parser::ParseDefineStatement()
{
    Expect(TokenType_Symbol, "define");

    auto location = ParseResourceLocation();

    Expect(TokenType_Other, "(");

    std::vector<std::string> parameters;

    while (!SkipIf(TokenType_Other, ")"))
    {
        parameters.emplace_back(Expect(TokenType_Symbol).Value);
        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }

    std::vector<ResourceLocation> tags;
    if (At(TokenType_Other, "#"))
    {
        while (!At(TokenType_Other, "{"))
        {
            Expect(TokenType_Other, "#");
            tags.emplace_back(ParseResourceLocation());

            if (!At(TokenType_Other, "{"))
                Expect(TokenType_Other, ",");
        }
    }

    Expect(TokenType_Other, "{");

    std::vector<ExpressionPtr> expressions;

    while (!SkipIf(TokenType_Other, "}"))
    {
        expressions.emplace_back(ParseExpression());
        Expect(TokenType_Other, ";");
    }

    return std::make_unique<DefineStatement>(location, parameters, tags, std::move(expressions));
}

mcc::ExpressionPtr mcc::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseActionExpression(), ParseActionExpression, 0);
}

mcc::ExpressionPtr mcc::Parser::ParseBoolExpression()
{
    const auto value = ExpectEnum("true", "false").Value;
    return std::make_unique<BoolExpression>(value == "true");
}

mcc::ExpressionPtr mcc::Parser::ParseIntegerExpression()
{
    const auto value = Expect(TokenType_Integer).Value;
    return std::make_unique<IntegerExpression>(std::stoll(value));
}

mcc::ExpressionPtr mcc::Parser::ParseFloatExpression()
{
    const auto value = Expect(TokenType_Float).Value;
    return std::make_unique<FloatExpression>(std::stod(value));
}

mcc::ExpressionPtr mcc::Parser::ParseStringExpression()
{
    auto value = Expect(TokenType_String).Value;
    return std::make_unique<StringExpression>(value);
}

mcc::ExpressionPtr mcc::Parser::ParseSymbolExpression()
{
    auto id = Expect(TokenType_Symbol).Value;
    return std::make_unique<SymbolExpression>(id);
}

mcc::ExpressionPtr mcc::Parser::ParseTargetExpression()
{
    auto id = Expect(TokenType_Target).Value;
    return std::make_unique<TargetExpression>(id);
}

mcc::ExpressionPtr mcc::Parser::ParseArrayExpression()
{
    Expect(TokenType_Other, "[");

    std::vector<ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "]"))
    {
        elements.emplace_back(ParseExpression());
        if (!At(TokenType_Other, "]"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ArrayExpression>(std::move(elements));
}

mcc::ExpressionPtr mcc::Parser::ParseObjectExpression()
{
    Expect(TokenType_Other, "{");

    std::map<std::string, ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = Expect(TokenType_Symbol).Value;

        Expect(TokenType_Other, ":");

        auto value = ParseExpression();

        elements.emplace(std::move(key), std::move(value));

        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ObjectExpression>(std::move(elements));
}

mcc::ExpressionPtr mcc::Parser::ParseFormatExpression()
{
    auto format = Expect(TokenType_FormatString).Value;

    std::vector<FormatNodePtr> nodes;

    for (size_t pos; (pos = format.find("${")) != std::string::npos;)
    {
        if (pos != 0)
            nodes.emplace_back(std::make_unique<StringNode>(format.substr(0, pos)));

        format = format.substr(pos + 2);
        std::stringstream stream(format);

        Parser parser(stream, "none");
        auto expression = parser.ParseExpression();
        nodes.emplace_back(std::make_unique<ExpressionNode>(std::move(expression)));

        format = format.substr(stream.gcount() + 1);
    }

    return std::make_unique<FormatExpression>(std::move(nodes));
}

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (At(TokenType_Symbol, "true") || At(TokenType_Symbol, "false"))
        return ParseBoolExpression();

    if (At(TokenType_Integer))
        return ParseIntegerExpression();

    if (At(TokenType_Float))
        return ParseFloatExpression();

    if (At(TokenType_String))
        return ParseStringExpression();

    if (At(TokenType_Symbol))
        return ParseSymbolExpression();

    if (At(TokenType_Target))
        return ParseTargetExpression();

    if (At(TokenType_Other, "["))
        return ParseArrayExpression();

    if (At(TokenType_Other, "{"))
        return ParseObjectExpression();

    if (At(TokenType_FormatString))
        return ParseFormatExpression();

    if (SkipIf(TokenType_Other, "~"))
        return std::make_unique<RelativeOffsetExpression>();

    if (SkipIf(TokenType_Other, "^"))
        return std::make_unique<LocalOffsetExpression>();

    throw std::runtime_error(
        std::format(
            "{}({},{}): cannot parse {} '{}'",
            m_Token.Where.Filename,
            m_Token.Where.Row,
            m_Token.Where.Col,
            m_Token.Type,
            m_Token.Value));
}

mcc::ExpressionPtr mcc::Parser::ParseActionExpression()
{
    auto actor = ParsePrimaryExpression();

    while (actor)
    {
        if (SkipIf(TokenType_Other, "("))
        {
            std::vector<ExpressionPtr> arguments;
            while (!SkipIf(TokenType_Other, ")"))
            {
                arguments.emplace_back(ParseExpression());
                if (!At(TokenType_Other, ")"))
                    Expect(TokenType_Other, ",");
            }
            actor = std::make_unique<CallExpression>(std::move(actor), std::move(arguments));
            continue;
        }

        break;
    }

    return actor;
}

mcc::ExpressionPtr mcc::Parser::ParseBinaryExpression(
    ExpressionPtr left,
    ExpressionPtr (Parser::*next)(),
    const unsigned min_pre)
{
    static const std::map<std::string, unsigned> pres
    {
        {"=", 0},
        {"+", 1},
        {"-", 1},
        {"*", 2},
        {"/", 2},
        {"%", 2},
    };

    auto has_pre = [this]
    {
        return pres.contains(m_Token.Value);
    };

    auto get_pre = [this]
    {
        return pres.at(m_Token.Value);
    };

    while (At(TokenType_Operator) && has_pre() && get_pre() >= min_pre)
    {
        const auto pre = get_pre();
        auto operator_ = Skip().Value;
        auto right = (this->*next)();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > pre || (!get_pre() && !pre)))
            right = ParseBinaryExpression(std::move(right), next, pre + (get_pre() > pre ? 1 : 0));
        left = std::make_unique<BinaryExpression>(operator_, std::move(left), std::move(right));
    }

    return left;
}
