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

    if (At(TokenType::Symbol))
    {
        auto name = Skip().Value;

        if (name == "void")
            return m_Context.GetVoid();
        if (name == "number")
            return m_Context.GetNumber();
        if (name == "string")
            return m_Context.GetString();
        if (name == "array")
            return m_Context.GetAnyArray();
        if (name == "object")
            return m_Context.GetAnyObject();
        if (name == "function")
            return m_Context.GetAnyFunction();

        if (auto type = m_Context.GetNamed(name))
            return type;

        Error(where, "undefined type {}", name);
    }

    if (SkipIf(TokenType::Other, "{"))
    {
        std::map<std::string, TypePtr> elements;

        while (!At(TokenType::Other, "}") && !At(TokenType::EoF))
        {
            auto name = Expect(TokenType::Symbol).Value;
            Expect(TokenType::Other, ":");
            elements[name] = ParseType();

            if (!At(TokenType::Other, "}"))
                Expect(TokenType::Other, ",");
        }
        Expect(TokenType::Other, "}");

        return m_Context.GetObject(elements);
    }

    if (SkipIf(TokenType::Other, "["))
    {
        std::vector<TypePtr> elements;

        while (!At(TokenType::Other, "]") && !At(TokenType::EoF))
        {
            elements.push_back(ParseType());

            if (!At(TokenType::Other, "]"))
                Expect(TokenType::Other, ",");
        }
        Expect(TokenType::Other, "]");

        return m_Context.GetTuple(elements);
    }

    if (SkipIf(TokenType::Other, "$"))
    {
        std::vector<TypePtr> parameters;

        const auto throws = SkipIf(TokenType::Other, "!");

        Expect(TokenType::Other, "(");
        while (!At(TokenType::Other, ")") && !At(TokenType::EoF))
        {
            parameters.push_back(ParseType());

            if (!At(TokenType::Other, ")"))
                Expect(TokenType::Other, ",");
        }
        Expect(TokenType::Other, ")");
        Expect(TokenType::Operator, "=>");

        const auto result = ParseType();

        return m_Context.GetFunction(parameters, result, throws);
    }

    if (SkipIf(TokenType::Other, "("))
    {
        auto type = ParseType();
        Expect(TokenType::Other, ")");
        return type;
    }

    Error(where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}

mcc::TypePtr mcc::Parser::ParseArrayType()
{
    auto base = ParseBaseType();

    while (SkipIf(TokenType::Other, "["))
    {
        Expect(TokenType::Other, "]");
        base = m_Context.GetArray(base);
    }

    return base;
}

mcc::TypePtr mcc::Parser::ParseUnionType()
{
    auto type = ParseArrayType();

    if (SkipIf(TokenType::Other, "|"))
    {
        std::set<TypePtr> elements;
        elements.insert(type);

        do
            elements.insert(ParseArrayType());
        while (SkipIf(TokenType::Other, "|"));

        type = m_Context.GetUnion(elements);
    }

    return type;
}
