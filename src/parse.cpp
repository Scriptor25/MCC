#include <format>
#include <iostream>
#include <mcc/parse.hpp>

bool mcc::parser_t::at(const token_type type, const std::string &value) const
{
    if (token.type != type)
        return false;

    if (!value.empty() && token.value != value)
        return false;

    return true;
}

bool mcc::parser_t::skip_if(const token_type type, const std::string &value)
{
    if (token.type != type)
        return false;

    if (!value.empty() && token.value != value)
        return false;

    token = lex(stream);
    return true;
}

mcc::token_t mcc::parser_t::skip()
{
    auto t = token;
    token = lex(stream);

    return t;
}

mcc::token_t mcc::parser_t::expect(token_type type, std::string value)
{
    if (token.type != type)
        throw std::runtime_error(std::format("expected {}, but is {}", type, token.type));

    if (!value.empty() && token.value != value)
        throw std::runtime_error(std::format("expected '{}', but is '{}'", value, token.value));

    auto t = token;
    token = lex(stream);

    return t;
}

mcc::statement_ptr_t mcc::parser_t::operator()()
{
    return parse_statement();
}

mcc::statement_ptr_t mcc::parser_t::parse_statement()
{
    while (token.type == token_type_eol)
        token = lex(stream);
    if (token.type == token_type_eof)
        return {};

    if (at(token_type_symbol, "namespace"))
        return parse_namespace_statement();
    if (at(token_type_symbol, "define"))
        return parse_define_statement();

    return parse_expression();
}

mcc::statement_ptr_t mcc::parser_t::parse_namespace_statement()
{
    expect(token_type_symbol, "namespace");

    auto id = expect(token_type_symbol).value;

    return std::make_unique<namespace_statement_t>(id);
}

mcc::statement_ptr_t mcc::parser_t::parse_define_statement()
{
    expect(token_type_symbol, "define");

    auto id = expect(token_type_symbol).value;

    expect(token_type_operator, "(");

    std::vector<std::string> parameters;

    while (!skip_if(token_type_operator, ")"))
    {
        parameters.emplace_back(expect(token_type_symbol).value);
        if (!at(token_type_operator, ")"))
            expect(token_type_operator, ",");
    }

    expect(token_type_operator, "{");

    std::vector<statement_ptr_t> statements;

    while (!skip_if(token_type_operator, "}"))
        statements.emplace_back(parse_statement());

    return std::make_unique<define_statement_t>(id, parameters, std::move(statements));
}

mcc::expression_ptr_t mcc::parser_t::parse_expression()
{
    throw std::runtime_error(std::format("cannot parse {} '{}'", token.type, token.value));
}
