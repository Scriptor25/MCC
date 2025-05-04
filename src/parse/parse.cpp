#include <algorithm>
#include <istream>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::Parser::Parser(std::istream &stream, std::string filename)
    : m_Stream(stream),
      m_Location(std::move(filename), 1, 0)
{
    Get();
    Next();
}

size_t mcc::Parser::Count() const
{
    return m_Count - m_Token.RawValue.size();
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
    m_Count++;
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

bool mcc::Parser::AtAny(const std::vector<TokenType> &types) const
{
    return std::ranges::any_of(
        types,
        [this](auto &type)
        {
            return At(type);
        });
}

bool mcc::Parser::AtEnum(const std::vector<const char *> &values) const
{
    return std::ranges::any_of(
        values,
        [this](auto &value)
        {
            return At(TokenType_Symbol, value);
        });
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
    Assert(
        m_Token.Type == type,
        m_Token.Where,
        "expected {}, but is {}",
        type,
        m_Token.Type);

    Assert(
        value.empty() || m_Token.Value == value,
        m_Token.Where,
        "expected '{}', but is '{}'",
        value,
        m_Token.Value);

    return Skip();
}

mcc::Token mcc::Parser::ExpectAny(const std::vector<TokenType> &types)
{
    for (auto &type: types)
        if (At(type))
            return Skip();

    Error(
        m_Token.Where,
        "expected {}, but is {}",
        types,
        m_Token.Type);
}

mcc::Token mcc::Parser::ExpectEnum(const std::vector<const char *> &values)
{
    for (auto &value: values)
        if (At(TokenType_Symbol, value))
            return Skip();

    Error(
        m_Token.Where,
        "expected {}, but is '{}'",
        values,
        m_Token.Value);
}
