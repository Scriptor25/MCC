#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/type.hpp>

mcc::TypePtr mcc::Parser::ParseType()
{
    return ParseUnionType();
}

mcc::TypePtr mcc::Parser::ParseBaseType()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Symbol))
    {
        auto name = Skip().Value;

        if (name == "void")
            return m_Context.GetVoid();
        if (name == "null")
            return m_Context.GetNull();
        if (name == "boolean")
            return m_Context.GetBoolean();
        if (name == "number")
            return m_Context.GetNumber();
        if (name == "string")
            return m_Context.GetString();

        if (auto type = m_Context.GetNamed(name))
            return type;

        Error(where, "undefined type {}", name);
    }

    if (SkipIf(TokenType_Other, "{"))
    {
        std::map<std::string, TypePtr> elements;

        while (!At(TokenType_Other, "}") && !At(TokenType_EOF))
        {
            auto name = Expect(TokenType_Symbol).Value;
            Expect(TokenType_Other, ":");
            elements[name] = ParseType();

            if (!At(TokenType_Other, "}"))
                Expect(TokenType_Other, ",");
        }
        Expect(TokenType_Other, "}");

        return m_Context.GetStruct(elements);
    }

    if (SkipIf(TokenType_Other, "["))
    {
        std::vector<TypePtr> elements;

        while (!At(TokenType_Other, "]") && !At(TokenType_EOF))
        {
            elements.emplace_back(ParseType());

            if (!At(TokenType_Other, "]"))
                Expect(TokenType_Other, ",");
        }
        Expect(TokenType_Other, "]");

        return m_Context.GetTuple(elements);
    }

    if (SkipIf(TokenType_Other, "("))
    {
        auto type = ParseType();
        Expect(TokenType_Other, ")");
        return type;
    }

    Error(where, "TODO");
}

mcc::TypePtr mcc::Parser::ParseArrayType()
{
    auto base = ParseBaseType();

    while (SkipIf(TokenType_Other, "["))
    {
        Expect(TokenType_Other, "]");
        base = m_Context.GetArray(base);
    }

    return base;
}

mcc::TypePtr mcc::Parser::ParseUnionType()
{
    auto type = ParseArrayType();

    if (SkipIf(TokenType_Operator, "|"))
    {
        std::set<TypePtr> elements;
        elements.emplace(type);

        do
            elements.emplace(ParseArrayType());
        while (SkipIf(TokenType_Operator, "|"));

        type = m_Context.GetUnion(elements);
    }

    return type;
}
