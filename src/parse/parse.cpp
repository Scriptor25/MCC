#include <istream>
#include <set>
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
