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
            return TypeContext::GetVoid();
        if (name == "null")
            return TypeContext::GetNull();
        if (name == "boolean")
            return TypeContext::GetBoolean();
        if (name == "number")
            return TypeContext::GetNumber();
        if (name == "string")
            return TypeContext::GetString();

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

        return TypeContext::GetStruct(elements);
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

        return TypeContext::GetTuple(elements);
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
        base = TypeContext::GetArray(base);
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

        type = TypeContext::GetUnion(elements);
    }

    return type;
}
